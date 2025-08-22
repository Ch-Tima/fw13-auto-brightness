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
static std::atomic<uint16_t> loopDelayMs {500};

//Возрощает текущие состояния il_value
static int method_get_illuminance(sd_bus_message *msg, void *, sd_bus_error *) {
    const uint16_t v = il_value.load();
    std::cout << "[D-BUS] GetIlluminance called, value=" << v << std::endl;
    return sd_bus_reply_method_return(msg, "q", v); // "q" = uint16
}

static int method_get_loopDelayMs(sd_bus_message *msg, void *, sd_bus_error *){
    const uint16_t v = loopDelayMs.load();
    std::cout << "[D-BUS] GetLoopDelayMs called, value=" << v << std::endl;
    return sd_bus_reply_method_return(msg, "q", v); // "q" = uint16
}

static const sd_bus_vtable demo_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("GetIlluminance", "", "q", method_get_illuminance, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("GetLoopDelayMs", "", "q", method_get_loopDelayMs, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

void do_work(){
    //do_work работа над получением in_illuminance_raw и изменения яркости экрана 
    std::cout << "START main loop" << std::endl;
    
    string line;
    int r;
    sd_bus *client_bus = nullptr;

    r = sd_bus_open_user(&client_bus);
    if (r < 0) {
        std::cerr << "Failed to open client bus: " << strerror(-r) << std::endl;
        return;
    }

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


        const uint16_t il = il_value.load();
        const uint16_t old = old_value.load();
        const uint8_t lim = new_limit.load();

        if (il > old + lim || il < old - lim) {
            if (count_check.load() >= number_of_check.load()) {
                old_value = il;

                // Готовим чистые error/reply перед каждым вызовом
                sd_bus_error error = SD_BUS_ERROR_NULL;
                sd_bus_message *reply = nullptr;

                // используем client_bus!
                r = sd_bus_call_method(
                    client_bus,
                    "org.kde.Solid.PowerManagement",
                    "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                    "org.kde.Solid.PowerManagement.Actions.BrightnessControl",
                    "setBrightnessSilent",
                    &error,
                    &reply,
                    "i",
                    (int)cal(il)  // сигнатура "i" → int
                );

                if (r < 0) {
                    std::cerr << "Error calling setBrightnessSilent: "
                              << strerror(-r) << " ("
                              << (error.message ? error.message : "no error msg")
                              << ")\n";
                }

                if (reply) {
                    sd_bus_message_unref(reply);
                    reply = nullptr;
                }
                sd_bus_error_free(&error);

                count_check = 0;
            } else {
                count_check++;
            }
        } else {
            std::cout << "OLD VALUE" << std::endl;
        }
        
        take = take-1;

        std::cout << "il_lum:" << static_cast<int>(il_value.load()) << std::endl;
        std::cout << "TAKE:" << static_cast<int>(take.load()) << std::endl;
        this_thread::sleep_for(chrono::milliseconds(loopDelayMs));// Wait 0.5 second before next read
    }
    sd_bus_unref(client_bus);// Закрываем клиентскую шину воркера

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

    auto w = std::async(std::launch::async, do_work);
    
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

    w.wait();
    return OK;
}