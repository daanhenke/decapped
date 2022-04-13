#pragma once

#include <types.hh>

typedef struct
{
    uintptr_t dest;
} guest_pagetable_entry_t;

typedef union
{
    struct
    {
        uint32_t offset: 12;
        uint32_t p2: 9;
        uint32_t p1: 9;
        uint32_t p0: 2;
    };
    uint32_t full;
} address_translation_t __attribute__((packed));

bool guest_memory_init();
