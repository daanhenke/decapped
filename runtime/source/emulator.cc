#include <emulator.hh>
#include <memory.hh>

emu_ctx_t* g_ctx = nullptr;

emu_ctx_t* emulator_get_context()
{
    return g_ctx;
}

void emulator_init()
{
    g_ctx = malloc<emu_ctx_t>(1, allocation_type_t::runtime);
    g_ctx->core_count = 8;
    g_ctx->cores = malloc<core_ctx_t*>(g_ctx->core_count, allocation_type_t::runtime);

    for (auto i = 0; i < g_ctx->core_count; i++)
    {
        emulator_init_core(i);
    }
}

void emulator_init_core(uint8_t index)
{
    auto core = malloc<core_ctx_t>(1, allocation_type_t::runtime);
    core->rax = core->rbx = core->rcx = core->rdx = core->rdi = core->rsi = core->rbp = core->rsp = core->rip = core->rflags = 0xAA;
    g_ctx->cores[index] = core;
}
