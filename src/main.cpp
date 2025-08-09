#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

int main(){
    string line;
    short take = 256;
    short value = 0;
    while(take > 1){
        ifstream mfile("/sys/bus/iio/devices/iio:device0/in_illuminance_raw");
        if(mfile.is_open()){
            while(getline(mfile, line)){
                value = std::stoi(line);
                cout << value*100 << '\n';
            }
            mfile.close();
        }
        else cout << "Unable to open file\n";
        take--;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    return 0;
}