#include <macros.hh>
#include <stdint.h>

import_func void log_string(const char* message);
import_func void log_hex(uint64_t number, const char* prefix);
import_func char* itoa(int32_t number, char* dest, uint8_t radix);

import_func void* memset(void*, uint32_t, uint32_t);
