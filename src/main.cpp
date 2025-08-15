#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

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
    uint8_t size = 8;
    vec2_u32 v[size] = {
        { 0,  500   }, 
        { 80,  3000   }, 
        { 100,  4500  }, 
        { 200,  5000  },   
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

int main(){



    sd_bus *bus = nullptr;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = nullptr;
    int r = sd_bus_open_user(&bus);//get bus
    if (r < 0) {//check bus
        std::cerr << "Failed to connect to user bus: " << strerror(-r) << "\n";
        return 1;
    }
    


    string line;
    uint8_t take = UINT8_MAX;
    uint8_t new_limit = 50;
    uint8_t number_of_check = 3;
    uint8_t count_check = 0;
    uint16_t old_value = UINT16_MAX;// Illuminance sensor old value
    uint16_t il_value = 0;// Illuminance sensor value

    while(take > 1){// Loop to periodically read illuminance sensor value
        //cout << take << ")\n";
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            getline(mfile, line);
            mfile.close();
            to_unit16t r = stringToUint16t(line);
            if(r.status == OK){
                il_value = r.value;
                cout << "illuminance value: " << il_value << '\n';// Print illuminance value
                cout << "cal(il_value): " << cal(il_value) << '\n';
            }else {
                cout << "ERROR: " << r.status << " set default value in il_value to 0" << '\n';
                il_value = 0;
            }
        }
        else cout << "Unable to open file\n";

        if(il_value > old_value+new_limit || il_value < old_value-new_limit){
            if(count_check >= number_of_check){
                cout << "new!\n";
                old_value = il_value;
                r = sd_bus_call_method(
                    bus,
                    "org.kde.Solid.PowerManagement",
                    "/org/kde/Solid/PowerManagement/Actions/BrightnessControl",
                    "org.kde.Solid.PowerManagement.Actions.BrightnessControl",
                    "setBrightnessSilent",
                    &error, &msg,
                    "i",
                    cal(il_value)
                );
            
            
                if (r < 0) {
                    std::cerr << "Error calling SetBrightness: " << strerror(-r) << "\n";
                }


                count_check = 0;
            }else {
                count_check++;
            }

        }else { 
            cout << "old!\n";
        }

        take--;

        cout << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));// Wait 0.5 second before next read
    }
    return OK;
}