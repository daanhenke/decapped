#include <types.hh>
#include <imports.hh>
#include <backend.hh>
#include <runtime.hh>

const char* backend_get_name()
{
    return "interpreter";
}

status_t backend_init()
{
    return 0x420;
}

status_t backend_tick(uint8_t core_index)
{
    auto cpu = cpu_get_context();
    auto core = cpu->cores[core_index];

    auto instr = decode_current_instruction(core_index);
    if (instr.opcode == opcode_t::unknown) return mkerror(0);

    log_instruction(instr);

    if (instr.opcode == opcode_t::jmp)
    {
        switch (instr.args[0].type)
        {
        case argument_type_t::rel8: core->rip += (instr.args[0].value + instr.length); break;
        default: break;
        }
    }

    if (instr.opcode == opcode_t::cli)
    {
        core->rip += instr.length;
    }

    return 0;
}
