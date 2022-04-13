#pragma once

#include <types.hh>
#include <macros.hh>

enum class allocation_type_t : uint8_t
{
    generic,
    runtime,
    page_table,
    guest_memory
};

void* malloc(uint32_t size, allocation_type_t type = allocation_type_t::generic);
void free(void* base);

template <typename T>
T* malloc(uint32_t count = 1, allocation_type_t type = allocation_type_t::generic)
{
    return reinterpret_cast<T*>(malloc(sizeof(T) * count, type));
}

void malloc_init();

export_func void* __imp_malloc(uint32_t size);
export_func void __imp_free(void* base);
