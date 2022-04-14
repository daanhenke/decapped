#pragma once

#include <types.hh>

uint64_t strlen(const char* string);
char* strcat(char* destination, const char* source);
char* ultoa(uint64_t value, char* destination, int radix);
