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

int main(){

    uint8_t size = 4;
    vec2_u32 v[size] = {
        { 0.0, 0.0 }, 
        { 50, 32768 },
        { 600, 55705 },
        { 3355, 65535 },
    };

    double mX = 49;
    double nowY = 0;
    double m = 0;

    for(int i = 1; i < size; i++){
        if(mX > v[i-1].x && mX <= v[i].x){
            cout << v[i-1].x << " | " << v[i].x << '\n';
            m = (v[i].y - v[i-1].y)/(v[i].x - v[i-1].x);
            cout << "m: " << m << '\n';
            nowY = m*mX-v[i-1].x;
            cout << "nowY: " << nowY << '\n';
            break;
        }
        cout << "i" << i << '\n';
    }

    return OK;

    string line;
    uint8_t take = UINT8_MAX;
    uint16_t il_value = 0;// Illuminance sensor value
    uint16_t max_br = 0;// Maximum brightness value

    ifstream max_br_file("/sys/class/backlight/amdgpu_bl1/max_brightness");
    if(max_br_file.is_open()){
        getline(max_br_file, line);
        max_br_file.clear();
        to_unit16t r = stringToUint16t(line);
        if(r.status == OK)
            max_br = r.value;
        else {
            cout << "ERROR: " << r.status << " set default value in max_br to" << UINT16_MAX << '\n';
            max_br = UINT16_MAX;
        }
    }

    cout << "max_brightness: " << max_br << '\n'; 

    while(take > 1){// Loop to periodically read illuminance sensor value
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            getline(mfile, line);
            mfile.close();
            to_unit16t r = stringToUint16t(line);
            if(r.status == OK){
                il_value = r.value;
                cout << il_value << '\n';// Print illuminance value
            }else {
                cout << "ERROR: " << r.status << " set default value in il_value to 0" << '\n';
                il_value = 0;
            }
        }
        else cout << "Unable to open file\n";
        take--;
        this_thread::sleep_for(chrono::milliseconds(1000));// Wait 1 second before next read
    }
    return OK;
}