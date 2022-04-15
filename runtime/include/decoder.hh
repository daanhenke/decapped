#pragma once

#include <runtime.hh>

typedef union
{
    uint8_t value;
    struct {
        uint8_t rm:     3;
        uint8_t reg:    3;
        uint8_t mod:    2;
    };
} modrm_t;

instruction_t decode_instruction(uint8_t* instruction, uintptr_t address);
export_func instruction_t decode_current_instruction(core_ctx_t* core);
