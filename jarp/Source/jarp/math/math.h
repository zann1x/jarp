#pragma once

#include "jarp/api_types.h"

#define _USE_MATH_DEFINES
#include <math.h>

/*
====================
math_max
====================
*/
inline static uint32_t math_max(uint32_t num1, uint32_t num2) {
    return (num1 > num2 ? num1 : num2);
}

/*
====================
math_min
====================
*/
inline static uint32_t math_min(uint32_t num1, uint32_t num2) {
    return (num1 < num2 ? num1 : num2);
}
