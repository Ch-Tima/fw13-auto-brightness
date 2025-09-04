#include <atomic>
#include <mutex>
#include <iostream>
#include <vector>
#include "vec2_u16.h"
#include <fstream>
#include "INIReader.h"

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

    bool loadFromIni(const std::string& filename);
    bool saveToIni(const std::string& filename);
    bool createDefault(const std::string& filename);
};
