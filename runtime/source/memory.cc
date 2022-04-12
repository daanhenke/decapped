#include <memory.hh>
#include <imports.hh>
#include <stdint.h>
#include <macros.hh>

uintptr_t g_dynamic_memory_base = 0;

typedef struct
{
    uint32_t max_pages;
    uint32_t used_pages;
} malloc_ctx_t;

malloc_ctx_t* malloc_ctx = nullptr;

uintptr_t malloc_find_first_empty_page()
{
    uint64_t* base = nullptr;

    while (true)
    {
        bool found = true;
        for (auto i = 0; i < 0x1000 / sizeof(*base); i++)
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

void malloc_init()
{
    log_string("initializing memory allocator!\n");
    g_dynamic_memory_base = malloc_find_first_empty_page();
    malloc_ctx = reinterpret_cast<malloc_ctx_t*>(g_dynamic_memory_base);
    malloc_ctx->max_pages = 0x1000;
    malloc_ctx->used_pages = 0;
}

export_func void* malloc(uint32_t size)
{
    malloc_ctx->used_pages++;
    return nullptr;
}

export_func uintptr_t malloc_get_dynamic_base()
{
    return g_dynamic_memory_base;
}
