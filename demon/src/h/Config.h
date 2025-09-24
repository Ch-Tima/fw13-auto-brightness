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
