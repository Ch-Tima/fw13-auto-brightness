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

#include <atomic>    // std::atomic<T>
#include <unistd.h>  // fsync
#include <mutex>     // std::mutex
#include <cstdio>    // std::remove, std::rename
#include <vector>    // std::vector
#include "vec2_u16.h"
#include <iostream> 
#include <fstream>    // std::ofstream
#include <fcntl.h>
#include "INIReader.h"// INIReader

struct Config
{
    std::atomic<uint16_t> changeThreshold{50};
    std::atomic<uint8_t> validationCount{3};
    std::atomic<uint16_t> loopDelayMs {500};

    std::mutex brakePointsMutex;
    std::vector<vec2_u16> brakePoints
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

    std::mutex saveMutex;

    bool loadFromIni(const std::string& filename);
    bool saveToIni(const std::string& filename);
    bool saveToIniAtomic(const std::string& filename);
    bool createDefault(const std::string& filename);
};
