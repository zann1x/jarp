#pragma once

#include "api_types.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define ZERO_ARRAY(count, array_ptr) zero_size(count * sizeof(array_ptr[0]), array_ptr)
#define ZERO_STRUCT(instance) zero_size(sizeof(instance), &instance)

/*
====================
zero_size
====================
*/
static void zero_size(uint32_t size, void* ptr) {
    uint8_t* byte = (uint8_t*)ptr;
    while (size--) {
        *byte++ = 0;
    }
}
