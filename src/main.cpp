#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

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

struct vec2_u32
{
    uint32_t x = 0;
    uint32_t y = 0;
};

struct vecL3
{
    double L1 = 0;
    double L2 = 0;
    double L3 = 0;
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

vecL3 LY_point3(double mY, double aY, double bY, double cY){
    vecL3 v3;
    v3.L1 = ((mY-bY)/(aY-bY))*((mY-cY)/(aY-cY));
    v3.L2 = ((mY-aY)/(bY-aY))*((mY-cY)/(bY-cY));
    v3.L3 = ((mY-aY)/(cY-aY))*((mY-bY)/(cY-bY));

    return v3;
}

int main(){

    vec2_u32 a, b, c;
    a.y = 3355;
    a.x = 65535;

    b.y = 600;
    b.x = 55705;

    c.y = 50;
    c.x = 32768;

    double mY = 3000;
    vecL3 nL = LY_point3(mY, a.y, b.y, c.y);
    double mX = (a.x*nL.L1)+(b.x*nL.L2)+(c.x*nL.L3);

    cout << "mY:" << mY << '\n';
    cout << "mX:" << mX << '\n';


    mY = 3355;
    nL = LY_point3(mY, a.y, b.y, c.y);
    mX = (a.x*nL.L1)+(b.x*nL.L2)+(c.x*nL.L3);
    
    cout << "mY:" << mY << '\n';
    cout << "mX:" << mX << '\n';

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
}// (((4095 - 3355) / (4095 + 3355)) * 2) * 100 


/*
A: 3355  B: 65535 (MAX)
A: 600  B: 55705 (~75%)
A: 50 B: 32768 (~50%)
*/