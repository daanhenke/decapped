#include <cpu.hh>
#include <memory.hh>

cpu_ctx_t* g_ctx = nullptr;

cpu_ctx_t* cpu_get_context()
{
    return g_ctx;
}

export_func cpu_ctx_t* __imp_cpu_get_context()
{
    return cpu_get_context();
}

void cpu_init()
{
    g_ctx = malloc<cpu_ctx_t>(1, allocation_type_t::runtime);
    g_ctx->core_count = 8;
    g_ctx->cores = malloc<core_ctx_t*>(g_ctx->core_count, allocation_type_t::runtime);

    for (auto i = 0; i < g_ctx->core_count; i++)
    {
        cpu_init_core(i);
    }
}

void cpu_init_core(uint8_t index)
{
    auto core = malloc<core_ctx_t>(1, allocation_type_t::runtime);
    core->rax = core->rbx = core->rcx = core->rdx = core->rdi = core->rsi = core->rbp = core->rsp = core->rip = core->rflags = 0xDEADBEEF13370420;
    core->cs = core->ds = core->es = core->ss = core->fs = core->gs = 0x1337;
    g_ctx->cores[index] = core;
}

void cpu_core_set_rip(uint8_t core, uint64_t value)
{
    g_ctx->cores[core]->rip = value;
}

export_func void __imp_cpu_core_set_rip(uint8_t core, uint64_t value)
{
    g_ctx->cores[core]->rip = value;
}
