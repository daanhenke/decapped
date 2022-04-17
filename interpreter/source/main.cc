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

template <typename T>
inline T get_displacement8(core_ctx_t* core, arg_t* first_disp_arg)
{
    auto displacement = static_cast<int8_t>(first_disp_arg[1].value);
    switch (first_disp_arg->value)
    {
    case 0x6: return static_cast<T>(core->rbp) + displacement; break;

    default: break;
    }

    return 0;
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

inline void lea(core_ctx_t* core, instruction_t instruction)
{
    auto dest_reg = gp_reg_as<uint16_t>(core, instruction.args[0].value);

    if (instruction.args[1].type == argument_type_t::displacement8)
    {
        *dest_reg = get_displacement8<uint16_t>(core, &(instruction.args[1]));
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
    else if (instruction.args[0].type == argument_type_t::reg16)
    {
        auto dest_reg = gp_reg_as<uint16_t>(core, instruction.args[0].value);
        if (instruction.args[1].type == argument_type_t::imm16)
        {
            *dest_reg = instruction.args[1].value;
        }
        else if (instruction.args[1].type == argument_type_t::reg16)
        {
            auto source_reg = gp_reg_as<uint16_t>(core, instruction.args[1].value);
            *dest_reg = *source_reg;
        }
    }

    advance_rip(core, instruction);
}

inline void jmp(core_ctx_t* core, instruction_t instruction)
{
    switch (instruction.args[0].type)
    {
        case argument_type_t::rel8: core->rip += (instruction.args[0].value + instruction.length);      break;
        case argument_type_t::pointer: core->rip = instruction.args[0].value;                           break;
        default:                                                                                        break;
    }
}

inline void rep(core_ctx_t* core, instruction_t instruction)
{
    core->rep_mode = 1;
    core->first_rep_cycle = true;
    advance_rip(core, instruction);
}

inline void movs(core_ctx_t* core, instruction_t instruction)
{
    if (core->rep_mode == 1)
    {
        if (static_cast<uint16_t>(core->rcx) == 0)
        {
            core->rep_mode = 0;
        }
        else core->rcx--;
    }
    uint32_t ds = core->ds;
    uint32_t es = core->es;
    uint32_t si = static_cast<uint16_t>(core->rsi);
    uint32_t di = static_cast<uint16_t>(core->rdi);

    uint32_t source_addr = (ds << 4) + si;
    uint32_t dest_addr = (es << 4) + di;

    auto source = reinterpret_cast<uint16_t*>(guest_memory_translate(source_addr));
    auto dest = reinterpret_cast<uint16_t*>(guest_memory_translate(dest_addr));

    core->rsi++;
    core->rdi++;

    *dest = *source;

    if (core->rep_mode == 0) advance_rip(core, instruction);
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

    auto instr = decode_current_instruction(core);
    if (instr.opcode == opcode_t::unknown) return mkerror(0);

    if (core->rep_mode == 0 || core->first_rep_cycle)
    {
        core->first_rep_cycle = false;
        log_instruction(instr);
        log_string("\n");
    }

    switch(instr.opcode)
    {
        case opcode_t::_xor:    call_handler(_xor);
        case opcode_t::mov:     call_handler(mov);
        case opcode_t::movs:    call_handler(movs);
        case opcode_t::lea:    call_handler(lea);
        case opcode_t::jmp:     call_handler(jmp);
        case opcode_t::rep:     call_handler(rep);
        case opcode_t::cli:     call_handler(cli);
        case opcode_t::cld:     call_handler(cld);
        default: return mkerror(1);
    }

    return 0;
}
#undef call_handler
