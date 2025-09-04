#include "../h/Config.h"

bool Config::loadFromIni(const std::string& filename){
    std::cout << "Config >> loadFromIni\n";
    INIReader reader(filename);
    if (reader.ParseError() < 0) {
        std::cout << "Config >> Can't load " << filename << "\n";
        return false;
    }

    changeThreshold = reader.GetInteger("tuning", "changeThreshold", 50);
    changeThreshold = reader.GetInteger("tuning", "validationCount", 3);
    changeThreshold = reader.GetInteger("tuning", "loopDelayMs", 500);
    
    {
        std::lock_guard<std::mutex> lock(brakePointsMutex);
        brakePoints.clear();
        for(int i = 0; /**/; i++){
            std::string key = "brakePoints" + std::to_string(i);
            std::string val = reader.Get("brakePoints", key, "");
            if (val.empty()) break;
            uint16_t x, y;
            if(sscanf(val.c_str(), "%hu,%hu", &x, &y) == 2){
                brakePoints.push_back({x, y});
            }
        }
    }


    return true;
}

bool Config::saveToIni(const std::string& filename){
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "[tuning]\n";
    file << "changeThreshold=" << (int)changeThreshold.load() << "\n";
    file << "validationCount=" << (int)validationCount.load() << "\n";
    file << "loopDelayMs=" << (int)loopDelayMs.load() << "\n\n";

    file << "[brakePoints]\n";
    for(int i = 0; i < brakePoints.size(); i++){
        std::lock_guard<std::mutex> lock(brakePointsMutex);
        file << "brakePoints" << i << "=" << brakePoints[i].x << ',' << brakePoints[i].y << "\n";
    }
    return true;
}

bool Config::createDefault(const std::string& filename){
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << "[tuning]\n";
    file << "changeThreshold=50\n";
    file << "validationCount=3\n";
    file << "loopDelayMs=500\n\n";
    file << "[brakePoints]\n";
    file << "brakePoints0=0,10000\n";
    file << "brakePoints1=4059,10000\n";
    return true;
}