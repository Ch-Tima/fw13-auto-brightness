#pragma once

#include <vector>
#include "vec2_u16.h"

struct Config
{
    uint16_t changeThreshold;
    uint8_t validationCount;
    uint16_t loopDelayMs;
    std::vector<vec2_u16> brakePoints;
};