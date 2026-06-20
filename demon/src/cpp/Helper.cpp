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

#include "../h/Helper.h"

void h_toupper(std::string *out){

    std::transform(
        out->begin(), 
        out->end(), 
        out->begin(),
        [](unsigned char c) {
            return std::toupper(c);
        }
    );

}

bool h_equal_content(std::string v1, std::string v2){

    h_toupper(&v1);
    h_toupper(&v2);

    if(v1 == v2)
        return true;
    else return false;

}

// Converts a string to uint16_t with error handling
to_unit16t h_stringToUint16t(std::string s){
    to_unit16t r;
    try{
        int val = std::stoi(s);
        if(val >= 0 && val <= UINT16_MAX)
            r.value = val;
        else r.status = OUT_OF_RANGE;
    }catch(std::invalid_argument const& ex){
        r.status = INVALID_ARG;//Bad Request
    }catch (std::out_of_range const& ex){
        r.status = INVALID_ARG;//Range Not Satisfiable
    }
    return r;
}