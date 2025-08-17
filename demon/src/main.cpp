#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

#include <future>

#include <systemd/sd-bus.h>
#include <systemd/sd-device.h>

using namespace std;

#define OK 0
#define INVALID_ARG 1
#define OUT_OF_RANGE 2

// Structure to hold conversion result: value and status
struct to_unit16t
{
    uint16_t value = 0;
    uint8_t status = 0;
};

// Converts a string to uint16_t with error handling
to_unit16t stringToUint16t(string s){
    to_unit16t r;
    try{
        int val = std::stoi(s);
        if(val >= 0 && val <= UINT16_MAX)
            r.value = val;
        else r.status = OUT_OF_RANGE;
    }catch(std::invalid_argument const& ex){
        r.status = INVALID_ARG;//Bad Request
    }catch (std::out_of_range const& ex){
        r.status = INVALID_ARG;//Range Not Satisfiable
    }
    return r;
}

struct vec2_u32
{
    double x;
    double y;
};

uint16_t cal(double mX){
    uint8_t size = 9;
    vec2_u32 v[size] = {
        { 0,    500   }, 
        { 20,   3000  }, 
        { 80,   4000  }, 
        { 100,  5000  }, 
        { 200,  5500  },   
        { 300,  6000  },   
        { 500,  7000  },
        { 1400, 8500  },
        { 3355, 10000 },
    };

    double nowY = 0;
    double m = 0;

    for(int i = 1; i < size; i++){
        if(mX > v[i-1].x && mX <= v[i].x){
            //cout << v[i-1].x << " | " << v[i].x << '\n';
            m = (v[i].y - v[i-1].y)/(v[i].x - v[i-1].x);
            //cout << "m: " << m << '\n';
            nowY = v[i-1].y + m * (mX - v[i-1].x);
            //cout << "nowY: " << nowY << '\n';
            return nowY;
        }
        //cout << "i" << i << '\n';
    }

    if (mX < v[0].x)  return static_cast<uint16_t>(v[0].y);
    if (mX > v[size-1].x) return static_cast<uint16_t>(v[size-1].y);

    return 0;
}

static std::atomic<uint8_t> take{UINT8_MAX};
static std::atomic<uint8_t> new_limit{50};
static std::atomic<uint8_t> number_of_check{3};
static std::atomic<uint8_t> count_check{0};
static std::atomic<uint16_t> old_value{UINT16_MAX};// Illuminance sensor old value
static std::atomic<uint16_t> il_value{0};// Illuminance sensor value

static int method_get_illuminance(sd_bus_message *msg, void *userdata, sd_bus_error *ret_error){    
    std::cout << "[D-BUS] GetIlluminance called, value=" << il_value.load() << std::endl;
    return sd_bus_reply_method_return(msg, "q", il_value.load());
}

static const sd_bus_vtable demo_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("GetIlluminance", "", "q", method_get_illuminance, SD_BUS_VTABLE_UNPRIVILEGED),

    SD_BUS_VTABLE_END
};

void do_work(sd_bus *bus, sd_bus_message *msg, sd_bus_error error){
    std::cout << "START main loop" << std::endl;
    string line;
    int r;
    while(take > 1){// Loop to periodically read illuminance sensor value
        
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            getline(mfile, line);
            mfile.close();
            to_unit16t r = stringToUint16t(line);
            if(r.status == OK){
                il_value = r.value;
            }else {
                cerr << "ERROR: " << r.status << " set default value in il_value to 0" << '\n';
                il_value = 0;
            }
        }
        else std::cerr << "Unable to open file" << std::endl;;


        if(il_value.load() > old_value.load()+new_limit.load() || il_value.load() < old_value.load()-new_limit.load()){
            if(count_check.load() >= number_of_check.load()){
                old_value = il_value.load();
                r = sd_bus_call_method(
                    bus,
                    "org.kde.Solid.PowerManagement",
                    "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                    "org.kde.Solid.PowerManagement.Actions.BrightnessControl",
                    "setBrightnessSilent",
                    &error, &msg,
                    "i",
                    cal(il_value.load())
                );
            
                if (r < 0) {
                    std::cerr << "Error calling SetBrightness: " << strerror(-r) << "\n";
                }
                count_check = 0;
            }else {
                count_check++;
            }

        }else { 
            std::cout << "OLD VALUE" << std::endl;
        }


        take = take-1;

        std::cout << "il_lum:" << static_cast<int>(il_value.load()) << std::endl;
        std::cout << "TAKE:" << static_cast<int>(take.load()) << std::endl;
        this_thread::sleep_for(chrono::milliseconds(500));// Wait 0.5 second before next read
    }
}

int main(){

    std::cout << "START: ABI" << std::endl;

    sd_bus *bus = nullptr;
    sd_bus_slot *slot = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = nullptr;
    int r = sd_bus_open_user(&bus);//get bus
    if (r < 0) {//check bus
        std::cerr << "Failed to connect to user bus: " << strerror(-r) << "\n";
        return 1;
    }
    
    r = sd_bus_add_object_vtable(bus, &slot, 
        "/com/ct/AutoBrightness", 
        "com.ct.AutoBrightness", 
        demo_vtable, nullptr);

    if (r < 0) {
        std::cerr << "Failed to add object: " << strerror(-r) << std::endl;
        return 1;
    }

    r = sd_bus_request_name(bus, "com.ct.AutoBrightness", 0);
    if (r < 0) {
        std::cerr << "Failed to acquire service name: " << strerror(-r) << std::endl;
        return 1;
    }

    auto w = std::async(std::launch::async, do_work, bus, msg, error);
    
    while(take > 1){
        r = sd_bus_process(bus, nullptr);//проверяем наличие сообщений на D-Bus
        if (r > 0) continue;//если есть сообщение, оно уже обработано >> сразу к следующей итерации
        //ждем сообщения
        r = sd_bus_wait(bus, (uint64_t)-1);
        if (r < 0) { //проверяем наличие ошибок
            std::cerr << "Failed to wait on bus: " << strerror(-r) << std::endl;
            break;
        }
    }

    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return OK;
}