#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <future>
#include <mutex>

#include "h/Config.h"

#include <systemd/sd-bus.h>
#include <systemd/sd-device.h>

#include "h/vec2_u16.h"

using namespace std;

#define OK 0
#define INVALID_ARG 1
#define OUT_OF_RANGE 2

static const string CONFIG = "aib.conf";

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


static atomic<uint8_t> take{UINT8_MAX};
static atomic<uint16_t> changeThreshold{50};
static atomic<uint8_t> validationCount{3};
static atomic<uint8_t> count_check{0};
static atomic<uint16_t> old_value{UINT16_MAX};// Illuminance sensor old value
static atomic<uint16_t> il_value{0};// Illuminance sensor value
static atomic<uint16_t> loopDelayMs {500};

static mutex brakePointsMutex;
static vector<vec2_u16> brakePoints
{
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

uint16_t cal(double mX){
    std::lock_guard<std::mutex> lock(brakePointsMutex);
    double nowY = 0;
    double m = 0;
    for(int i = 1; i < brakePoints.size(); i++){
        if(mX > brakePoints[i-1].x && mX <= brakePoints[i].x){
            m = (brakePoints[i].y - brakePoints[i-1].y)/(brakePoints[i].x - brakePoints[i-1].x);
            nowY = brakePoints[i-1].y + m * (mX - brakePoints[i-1].x);
            return static_cast<uint16_t>(nowY);
        }
    }

    if (mX < brakePoints[0].x)  return static_cast<uint16_t>(brakePoints[0].y);
    if (mX > brakePoints[brakePoints.size()-1].x) return static_cast<uint16_t>(brakePoints[brakePoints.size()-1].y);

    return 0;
}

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

static int method_set_loopDelayMs(sd_bus_message *msg, void *, sd_bus_error *){
    uint16_t value;
    int r = sd_bus_message_read(msg, "q", &value);
    if(r < 0){
        return r;
    }

    std::cout << "[D-BUS] SetLoopDelayMs called, value=" << value << std::endl;
    loopDelayMs = value;

    return sd_bus_reply_method_return(msg, NULL);
}

//порог чувствительности: насколько новое значение (il) должно отличаться от старого (old)
static int method_get_changeThreshold(sd_bus_message *msg, void *, sd_bus_error *){
    const uint16_t v = changeThreshold.load();
    std::cout << "[D-BUS] GetChangeThreshold called, value=" << v << std::endl;
    return sd_bus_reply_method_return(msg, "q", v); // "q" = uint16
}

static int method_set_changeThreshold(sd_bus_message *msg, void *, sd_bus_error *){
    uint16_t value;
    int r = sd_bus_message_read(msg, "q", &value);
    if(r < 0){
        return r;
    }
    std::cout << "[D-BUS] SetChangeThreshold called, value=" << value << std::endl;
    changeThreshold = value;
    return sd_bus_reply_method_return(msg, NULL);
}

static int method_get_validationCount(sd_bus_message *msg, void *, sd_bus_error *){
    const uint8_t v = validationCount.load();
    std::cout << "[D-BUS] GetValidationCount called, value=" << v << std::endl;
    return sd_bus_reply_method_return(msg, "y", v); // "y" = uint8
}

static int method_set_validationCount(sd_bus_message *msg, void *, sd_bus_error *){
    uint8_t v;
    int r = sd_bus_message_read(msg, "y", &v);
    if(r < 0){
        return r;
    }
    std::cout << "[D-BUS] SetValidationCount called, value=" << v << std::endl;
    validationCount = v;
    return sd_bus_reply_method_return(msg, NULL);
}

static int method_get_brake_points(sd_bus_message *msg, void *, sd_bus_error *){
    std::lock_guard<std::mutex> lock(brakePointsMutex);
    std::cout << "[D-BUS] GetVectorBrakePoints called, value=" << brakePoints.size() << std::endl;
    sd_bus_message *reply = nullptr;//Создаём указатель под сообщение-ответ (reply). Пока nullptr.
    int r = sd_bus_message_new_method_return(msg, &reply);//Создаём новое сообщение-ответ на входное msg.
    if (r < 0) return r;//создался? 

    r = sd_bus_message_open_container(reply, 'a', "(qq)");//окрыть
    if (r < 0) return r;

    for (const vec2_u16 &p : brakePoints) {//писать
        r = sd_bus_message_append(reply, "(qq)", p.x, p.y);
        if (r < 0) return r;
    }

    r = sd_bus_message_close_container(reply);//закрыть контейнер
    if (r < 0) return r;

    r = sd_bus_send(NULL, reply, NULL);//отправка ответа (reply)
    sd_bus_message_unref(reply);//освобождаем память
    return r;//0 = успех, <0 = ошибка
}

static int method_set_brake_points(sd_bus_message *msg, void *, sd_bus_error *){

    std::vector<vec2_u16> value = {};
    int r = sd_bus_message_enter_container(msg, 'a', "(qq)");
    if(r < 0){
        std::cout << "[D-BUS] SetVectorBrakePoints > enter_container failed." << std::endl;
        return r;
    }

    while(sd_bus_message_at_end(msg, 0) == 0){
        vec2_u16 point;
        sd_bus_message_read(msg, "(qq)", &point.x, &point.y);
        value.push_back(point);
    }

    sd_bus_message_close_container(msg);

    std::cout << "[D-BUS] SetVectorBrakePoints called, value.size=" << value.size() << std::endl;
    
    {
        std::lock_guard<std::mutex> lock(brakePointsMutex);
        brakePoints = value;
    }

    return sd_bus_reply_method_return(msg, NULL);
}

static const sd_bus_vtable demo_vtable[] = {
    SD_BUS_VTABLE_START(0),
    //GetIlluminance
    SD_BUS_METHOD("GetIlluminance", "", "q", method_get_illuminance, SD_BUS_VTABLE_UNPRIVILEGED),
    //get|set LoopDelayMs
    SD_BUS_METHOD("GetLoopDelayMs", "", "q", method_get_loopDelayMs, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("SetLoopDelayMs", "q", "", method_set_loopDelayMs, SD_BUS_VTABLE_UNPRIVILEGED),
    //get|set ChangeThreshold
    SD_BUS_METHOD("GetChangeThreshold", "", "q", method_get_changeThreshold, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("SetChangeThreshold", "q", "", method_set_changeThreshold, SD_BUS_VTABLE_UNPRIVILEGED),
    //get|set ValidationCount
    SD_BUS_METHOD("GetValidationCount", "", "y", method_get_validationCount, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("SetValidationCount", "y", "", method_set_validationCount, SD_BUS_VTABLE_UNPRIVILEGED),
    //get|set VectorBrakePoints
    SD_BUS_METHOD("GetVectorBrakePoints", "", "a(qq)", method_get_brake_points, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("SetVectorBrakePoints", "a(qq)", "", method_set_brake_points, SD_BUS_VTABLE_UNPRIVILEGED),
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
        const uint8_t thr = changeThreshold.load();

        if (il > old + thr || il < old - thr) {
            if (count_check.load() >= validationCount.load()) {
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

    Config f;
    uint8_t itry = 0;
    while (!f.loadFromIni(CONFIG) && itry < 3)
    {
        if(!f.createDefault(CONFIG)){
            std::cout << "I can not create aib.conf!\n";
            this_thread::sleep_for(chrono::milliseconds(250));
        }else {
            std::cout << "Create default aib.conf!\n";
            break;
        }
        itry++;
    }
    
    //WORK

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

    if(f.saveToIni(CONFIG)){
        std::cout << "Successful save config.\n";
    }else  std::cout << "Failed save config.\n";

    w.wait();
    return OK;
}