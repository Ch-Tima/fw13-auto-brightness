#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

#define OK 0
#define INVALID_ARG 1
#define OUT_OF_RANGE 2

struct to_unit16t
{
    uint16_t value = 0;
    uint8_t status = 0;
};


to_unit16t stringToUint16t(string s){
    to_unit16t r;
    try{
        r.value = std::stoi(s);
    }catch(std::invalid_argument const& ex){
        r.status = INVALID_ARG;//Bad Request
    }catch (std::out_of_range const& ex){
        r.status = INVALID_ARG;//Range Not Satisfiable
    }
    return r;
}

int main(){
    string line;
    uint8_t take = UINT8_MAX;
    uint16_t il_value = 0;
    uint16_t max_br = 0;

    ifstream max_br_file("/sys/class/backlight/amdgpu_bl1/max_brightness");
    if(max_br_file.is_open()){
        while (getline(max_br_file, line)){
            to_unit16t r = stringToUint16t(line);
            if(r.status == OK)
                max_br = r.value;
            else {
                cout << "ERROR: " << r.status << " set default value in max_br to" << UINT16_MAX << '\n';
                max_br = UINT16_MAX;
            }
        }
        max_br_file.clear();
    }

    cout << "max_brightness: " << max_br << '\n'; 

    while(take > 1){
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            while(getline(mfile, line)){
                to_unit16t r = stringToUint16t(line);
                if(r.status == OK){
                    il_value = r.value;
                    cout << il_value << '\n';
                }else {
                    cout << "ERROR: " << r.status << " set default value in il_value to 0" << '\n';
                    il_value = 0;
                }
                
            }
            mfile.close();
        }
        else cout << "Unable to open file\n";
        take--;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    return OK;
}