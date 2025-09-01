#include "../h/Config.h"

bool Config::loadFromIni(const std::string& filename){
    return false;
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