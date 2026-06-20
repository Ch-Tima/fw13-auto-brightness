/*
 * fw13-auto-brightness
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

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <future>
#include <csignal>
#include <algorithm>


#define OK 0
#define INVALID_ARG 1
#define OUT_OF_RANGE 2

// Structure to hold conversion result: value and status
struct to_unit16t
{
    uint16_t value = 0;
    uint8_t status = 0;
};

// Converts a string to uint16_t with error handling
to_unit16t h_stringToUint16t(std::string s);

void h_toupper(std::string *out);

bool h_equal_content(std::string v1, std::string v2);