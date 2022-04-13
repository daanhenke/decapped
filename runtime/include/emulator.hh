#pragma once

#include <macros.hh>
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
} emu_ctx_t;

export_func emu_ctx_t* emulator_get_context();
void emulator_init();
void emulator_init_core(uint8_t index);
