#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

int main(){
    string line;
    uint8_t take = UINT8_MAX;
    uint16_t il_value = 0;
    uint16_t max_br = 0;

    ifstream max_br_file("/sys/class/backlight/amdgpu_bl1/max_brightness");
    if(max_br_file.is_open()){
        while (getline(max_br_file, line)){
            max_br = std::stoi(line);
        }
        max_br_file.clear();
    }

    cout << "max_brightness: " << max_br << '\n'; 

    while(take > 1){
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            while(getline(mfile, line)){
                il_value = std::stoi(line);
                cout << il_value << '\n';
            }
            mfile.close();
        }
        else cout << "Unable to open file\n";
        take--;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    return 0;
}