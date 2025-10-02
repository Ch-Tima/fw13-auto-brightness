// fw13-auto-brightness
// Copyright (C) 2025 <Ch-Tima>
// Licensed under AGPLv3 (https://www.gnu.org/licenses/)
#pragma once

#include <vector>
#include "vec2_u16.h"

struct Config
{
    uint16_t changeThreshold = 0;
    uint8_t validationCount = 0;
    uint16_t loopDelayMs = 0;
    std::vector<vec2_u16> brakePoints = {};
};