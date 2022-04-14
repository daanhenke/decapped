#pragma once

#include <runtime.hh>

instruction_t decode_instruction(uint8_t* instruction, uintptr_t address);
export_func instruction_t decode_current_instruction(uint8_t core_index);
