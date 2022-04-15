#include <types.hh>
#include <imports.hh>
#include <backend.hh>
#include <runtime.hh>

inline void advance_rip(core_ctx_t* core, instruction_t instruction)
{
    core->rip += instruction.length;
}

template <typename T>
inline T* gp_reg_as(core_ctx_t* core, uint8_t reg_idx)
{
    switch (reg_idx)
    {
    case 0x0: return reinterpret_cast<T*>(&core->rax);
    case 0x1: return reinterpret_cast<T*>(&core->rcx);
    case 0x2: return reinterpret_cast<T*>(&core->rdx);
    case 0x3: return reinterpret_cast<T*>(&core->rbx);
    case 0x4: return reinterpret_cast<T*>(&core->rsp);
    case 0x5: return reinterpret_cast<T*>(&core->rbp);
    case 0x6: return reinterpret_cast<T*>(&core->rsi);
    case 0x7: return reinterpret_cast<T*>(&core->rdi);
    default:  return nullptr;
    }
}

template <typename T>
inline T* seg_reg_as(core_ctx_t* core, uint8_t reg_idx)
{
    switch (reg_idx)
    {
    case 0x0: return reinterpret_cast<T*>(&core->es);
    case 0x1: return reinterpret_cast<T*>(&core->cs);
    case 0x2: return reinterpret_cast<T*>(&core->ss);
    case 0x3: return reinterpret_cast<T*>(&core->ds);
    case 0x4: return reinterpret_cast<T*>(&core->fs);
    case 0x5: return reinterpret_cast<T*>(&core->gs);
    default:  return nullptr;
    }
}

inline void _xor(core_ctx_t* core, instruction_t instruction)
{
    if (instruction.args[0].type == argument_type_t::reg16)
    {
        auto dest_reg = gp_reg_as<uint16_t>(core, instruction.args[0].value);
        auto source_reg = gp_reg_as<uint16_t>(core, instruction.args[1].value);
        *dest_reg = *dest_reg ^ *source_reg;
    }

    advance_rip(core, instruction);
}

inline void mov(core_ctx_t* core, instruction_t instruction)
{
    if (instruction.args[0].type == argument_type_t::sreg16)
    {
        auto dest_reg = seg_reg_as<uint16_t>(core, instruction.args[0].value);
        auto source_reg = gp_reg_as<uint16_t>(core, instruction.args[1].value);
        *dest_reg = *source_reg;
    }

    advance_rip(core, instruction);
}

inline void jmp(core_ctx_t* core, instruction_t instruction)
{
    switch (instruction.args[0].type)
    {
        case argument_type_t::rel8: core->rip += (instruction.args[0].value + instruction.length);      break;
        default:                                                                                        break;
    }
}

inline void cli(core_ctx_t* core, instruction_t instruction)
{
    // TODO: IMPLEMENT
    advance_rip(core, instruction);
}

inline void cld(core_ctx_t* core, instruction_t instruction)
{
    // TODO: IMPLEMENT
    advance_rip(core, instruction);
}

const char* backend_get_name()
{
    return "interpreter";
}

status_t backend_init()
{
    return 0x420;
}

#define call_handler(name) name(core, instr); break
status_t backend_tick(uint8_t core_index)
{
    auto cpu = cpu_get_context();
    auto core = cpu->cores[core_index];

    auto instr = decode_current_instruction(core_index);
    if (instr.opcode == opcode_t::unknown) return mkerror(0);

    log_instruction(instr);

    switch(instr.opcode)
    {
        case opcode_t::_xor:    call_handler(_xor);
        case opcode_t::mov:     call_handler(mov);
        case opcode_t::jmp:     call_handler(jmp);
        case opcode_t::cli:     call_handler(cli);
        case opcode_t::cld:     call_handler(cld);
    }

    return 0;
}
#undef call_handler