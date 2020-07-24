// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA.
//
//  This file is part of project: OpenRespirator
//

#include "cmath.h"

uint16_t isqrt(uint16_t x)  
{  
    uint16_t op, res, one;  
  
    op = x;  
    res = 0;  
  
    /* "one" starts at the highest power of four <= than the argument. */  
    one = 1 << 14;  /* second-to-top bit set */  
    while (one > op) one >>= 2;  
  
    while (one != 0) {  
        if (op >= res + one) {  
            op -= res + one;  
            res += one << 1;  // <-- faster than 2 * one  
        }  
        res >>= 1;  
        one >>= 2;  
    }  
    return res;  
}
