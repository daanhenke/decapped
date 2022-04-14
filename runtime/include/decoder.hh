#pragma once

#include <types.hh>

enum class opcode_t
{
    unknown,
    jmp,
    cli,
};

enum class argument_type_t
{
    none,
    rel8,
};

typedef struct
{
    argument_type_t type;
    uint64_t value;
} arg_t;

typedef struct
{
    opcode_t opcode;
    uintptr_t address;
    uint8_t length;
    arg_t args[2];
} instruction_t;

instruction_t decode_instruction(uint8_t* instruction, uintptr_t address);
instruction_t decode_current_instruction(uint8_t core_index);
