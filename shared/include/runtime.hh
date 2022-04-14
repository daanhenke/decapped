#pragma once

#include <macros.hh>
#include <types.hh>

#include <types.hh>

typedef struct
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
    uint64_t rflags;
} core_ctx_t;

typedef struct
{
    uint8_t core_count;
    core_ctx_t** cores;
} cpu_ctx_t;

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

#ifndef RUNTIME
import_func instruction_t decode_current_instruction(uint8_t core_index);
import_func void log_instruction(instruction_t instruction);
import_func cpu_ctx_t* cpu_get_context();
#endif
