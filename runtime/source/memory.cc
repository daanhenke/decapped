#include <memory.hh>
#include <imports.hh>
#include <stdint.h>
#include <macros.hh>

uintptr_t g_dynamic_memory_base = 0;

typedef struct
{
    uint32_t max_pages;
    uint32_t used_pages;
    uint8_t* use_bitmap;
    uint32_t* size_array;
    uint32_t current_index;
    uintptr_t dynamic_base;
    uint8_t* type_array;
} malloc_ctx_t;

malloc_ctx_t* malloc_ctx = nullptr;

uintptr_t malloc_find_first_empty_page()
{
    uint64_t* base = nullptr;

    while (true)
    {
        bool found = true;
        // TODO: CHANGE BACK TO 0x1000
        for (auto i = 0; i < 0x4000 / sizeof(*base); i++)
        {
            if (base[i] != 0)
            {
                base = &base[i + 1];
                found = false;
                break;
            }
        }

        if (found)
        {
            auto value = reinterpret_cast<uintptr_t>(base);
            auto masked = value & (~0xFFF);
            if (value != masked) return masked + 0x1000;
            return value;
        }
    }
}

#define offsetof(type, element) ((uintptr_t)&(((type*)0)->element))

bool does_index_fit(uint32_t index, uint32_t size)
{
    for (auto i = index; i < index + size; i++)
    {
        auto value = (malloc_ctx->use_bitmap[i / 8]);
        if (get_bit(value, (i % 8))) return false;
    }

    return true;
}

inline void set_bits(uint32_t index, uint32_t count, uint8_t value)
{
    auto i = 0;
    for (auto i = index; i < index + count; i++)
    {
        auto off = i % 8;
        auto byte = (malloc_ctx->use_bitmap[i / 8]);
        set_bit(byte, off, value);
        malloc_ctx->use_bitmap[i / 8] = byte;
    }
}

void* malloc(uint32_t size, allocation_type_t type)
{
    log_hex(static_cast<uint8_t>(type), "malloc type ");
    auto size_in_pages = size / 0x1000;
    if (size_in_pages * 0x1000 != size) size_in_pages++;

    auto index = malloc_ctx->current_index;
    while (true)
    {
        auto value = &(malloc_ctx->use_bitmap[index / 8]);
        if (does_index_fit(index, size_in_pages))
        {
            set_bits(index, size_in_pages, 1);
            malloc_ctx->size_array[index] = size_in_pages;
            malloc_ctx->type_array[index] = static_cast<uint8_t>(type);
            malloc_ctx->used_pages += size_in_pages;
            malloc_ctx->current_index = index;
            return reinterpret_cast<void*>(malloc_ctx->dynamic_base + index * 0x1000);
        }
        else
        {
            index++;
        }

        if (index > 0x100) break;
        index++;
    }
    return nullptr;
}

void free(void* base)
{
    auto index = (reinterpret_cast<uintptr_t>(base) - malloc_ctx->dynamic_base) / 0x1000;
    auto size = malloc_ctx->size_array[index];

    set_bits(index, size, 0);
    malloc_ctx->size_array[index] = 0;
    malloc_ctx->type_array[index] = 0;
    malloc_ctx->used_pages -= size;
    malloc_ctx->current_index = index;
}

void malloc_init()
{
    log_string("initializing memory allocator!\n");
    g_dynamic_memory_base = malloc_find_first_empty_page();

    malloc_ctx = reinterpret_cast<malloc_ctx_t*>(g_dynamic_memory_base);
    malloc_ctx->max_pages = 0x1000;
    malloc_ctx->used_pages = 0;
    malloc_ctx->current_index = 0;

    malloc_ctx->use_bitmap = reinterpret_cast<uint8_t*>(g_dynamic_memory_base + sizeof(malloc_ctx_t));
    auto bitmap_byte_count = malloc_ctx->max_pages / 8;
    malloc_ctx->size_array = reinterpret_cast<uint32_t*>(g_dynamic_memory_base + sizeof(malloc_ctx_t) + bitmap_byte_count);
    auto sizes_byte_count = sizeof(uint16_t) * malloc_ctx->max_pages;
    malloc_ctx->type_array = reinterpret_cast<uint8_t*>(g_dynamic_memory_base + sizeof(malloc_ctx_t) + bitmap_byte_count + sizes_byte_count);
    auto type_byte_count = malloc_ctx->max_pages;

    auto base = g_dynamic_memory_base + sizeof(malloc_ctx_t) + bitmap_byte_count + sizes_byte_count + type_byte_count;
    if (base / 0x1000 * 0x1000 != base) base = (base / 0x1000 + 1) * 0x1000;
    malloc_ctx->dynamic_base = base;

    for (auto bitmap_i = 0; bitmap_i < bitmap_byte_count; bitmap_i++)
    {
        malloc_ctx->use_bitmap[bitmap_i] = 0;
    }

    for (auto size_i = 0; size_i < malloc_ctx->max_pages; size_i++)
    {
        malloc_ctx->size_array[size_i] = 0;
        malloc_ctx->type_array[size_i] = 0;
    }
}

export_func uintptr_t malloc_get_dynamic_base()
{
    return g_dynamic_memory_base;
}

export_func void* __imp_malloc(uint32_t size)
{
    return malloc(size);
}

export_func void __imp_free(void* base)
{
    free(base);
}
