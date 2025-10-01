/*
 * fw13-auto-brightness\
 * Part of fw13-auto-brightness project (AutoBrightnessUI / AutoBrightnessIluminance)
 * Copyright (C) 2025  <Ch-Tima>
 *
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../h/Config.h"

bool Config::loadFromIni(const std::string& filename){
    std::cout << "Config >> loadFromIni\n";
    INIReader reader(filename);
    if (reader.ParseError() < 0) {
        std::cout << "Config >> Can't load " << filename << "\n";
        return false;
    }

    changeThreshold = reader.GetInteger("tuning", "changeThreshold", 50);
    validationCount = reader.GetInteger("tuning", "validationCount", 3);
    loopDelayMs = reader.GetInteger("tuning", "loopDelayMs", 500);
    
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

bool Config::saveToIniAtomic(const std::string& filename) {
    std::lock_guard<std::mutex> lock(saveMutex); // защита от параллельных вызовов
    std::string tmpFile = filename + ".tmp";
    {
        std::ofstream file(tmpFile);
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

        file.flush();//выталкиваем из буфера C++ в ОС
        //fileno(FILE*) обычно конвертирует FILE* > int

        // Получаем дескриптор через FILE* 
        FILE* fp = fopen(tmpFile.c_str(), "r");
        if (fp) {//запиши все данные и метаданные файла (размер, время изменения) на диск прямо сейчас.
            int fd = fileno(fp);
            if (fd != -1) fsync(fd);//выталкиваем из буфера ОС на диск
            fclose(fp);
        }
    }

    if (std::rename(tmpFile.c_str(), filename.c_str()) != 0) {
        std::perror("rename failed");
        std::remove(tmpFile.c_str());
        return false;
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