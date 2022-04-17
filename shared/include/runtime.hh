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
    uint16_t es;
    uint16_t cs;
    uint16_t ss;
    uint16_t ds;
    uint16_t fs;
    uint16_t gs;
    uint8_t rep_mode;
    bool first_rep_cycle;
} core_ctx_t;

typedef struct
{
    uint8_t core_count;
    core_ctx_t** cores;
} cpu_ctx_t;

enum class opcode_t
{
    unknown,
    _xor,
    movs,
    lea,
    mov,
    jmp,
    rep,
    cli,
    cld,
};

enum class argument_type_t
{
    none,
    rel8,
    imm16,
    reg16,
    sreg16,
    pointer,
    displacement8
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
    arg_t args[4];
} instruction_t;

#ifndef RUNTIME
import_func instruction_t decode_current_instruction(core_ctx_t* core);
import_func void log_instruction(instruction_t instruction, bool prefix = true);
import_func cpu_ctx_t* cpu_get_context();
import_func uintptr_t guest_memory_translate(uintptr_t address);
#endif
