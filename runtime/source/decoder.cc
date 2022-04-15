#include <decoder.hh>
#include <cpu.hh>
#include <guest_memory.hh>
#include <imports.hh>

void decode_rel8(uint8_t* code, arg_t* dest)
{
    dest->type = argument_type_t::rel8;
    dest->value = *code;
}

void decode_modrm(uint8_t* code, arg_t* start, bool reg_first = false, argument_type_t reg_type = argument_type_t::reg16, argument_type_t rm_type = argument_type_t::reg16)
{
    modrm_t modrm;
    modrm.value = *code++;

    if (reg_first)
    {
        start->type = reg_type;
        start->value = modrm.reg;
        start++;
    }

    switch (modrm.mod)
    {
    case 0b11:
        start->type = rm_type;
        start->value = modrm.rm;
        break;

    default:
        log_hex(modrm.mod, "unsupported modrm mod: ");
    }

    if (! reg_first)
    {
        start++;
        start->type = reg_type;
        start->value = modrm.reg;
    }
}

instruction_t decode_instruction(uint8_t* instruction, uintptr_t address)
{
    instruction_t result;
    result.address = address;
    result.opcode = opcode_t::unknown;
    result.args[0] = { argument_type_t::none, 0 };
    result.args[1] = { argument_type_t::none, 0 };

    auto instruction_start = instruction;
    auto opcode = *instruction++;
    switch (opcode)
    {
    case 0x31:  result.opcode = opcode_t::_xor; decode_modrm(instruction++, &(result.args[0])); break;
    case 0x8E:  result.opcode = opcode_t::mov; decode_modrm(instruction++, &(result.args[0]), true, argument_type_t::sreg16);  break;
    case 0xEB:  result.opcode = opcode_t::jmp; decode_rel8(instruction++, &(result.args[0]));   break;
    case 0xFA:  result.opcode = opcode_t::cli;                                                  break;
    case 0xFC:  result.opcode = opcode_t::cld;                                                  break;
    default:    log_hex(opcode, "unknown opcode: ");                                            break;
    }

    result.length = static_cast<uint8_t>(instruction - instruction_start);

    return result;
}

instruction_t decode_current_instruction(uint8_t core_index)
{
    auto cpu = cpu_get_context();
    auto core = cpu->cores[core_index];
    auto phys = guest_memory_translate(core->rip);
    return decode_instruction(reinterpret_cast<uint8_t*>(phys), core->rip);
}
