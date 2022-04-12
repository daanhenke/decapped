#include <memory.hh>
#include <imports.hh>
#include <stdint.h>
#include <macros.hh>

constexpr uintptr_t g_dynamic_memory_base = 0x4000;

typedef struct
{
    uint64_t max_pages;
    uint64_t used_pages;
} malloc_ctx_t;

malloc_ctx_t* malloc_ctx = nullptr;

void malloc_init()
{
    log_string("initializing memory allocator!\n");
    malloc_ctx = reinterpret_cast<malloc_ctx_t*>(g_dynamic_memory_base);
    malloc_ctx->max_pages = 13371338;
}

export_func uintptr_t malloc_get_dynamic_base()
{
    return g_dynamic_memory_base;
}
