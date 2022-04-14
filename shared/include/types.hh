#pragma once

#include <stdint.h>

typedef uint32_t status_t;

#define succeeded(status) (!(status & 0x80000000))
#define mkerror(status) (status | 0x80000000)
