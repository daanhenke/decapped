#pragma once

#define export_func __attribute__((visibility("default"))) extern "C"
#define import_func extern "C"

#define get_bit(number, index) ((number >> (index & 7)) & 1)
#define set_bit(number, index, value) (number = value ? number | (1 << index) : number & ~(1 << index))
