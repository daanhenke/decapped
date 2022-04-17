#include <decoder.hh>
#include <cpu.hh>
#include <guest_memory.hh>
#include <imports.hh>

void decode_rel8(uint8_t* code, arg_t* dest)
{
    dest->type = argument_type_t::rel8;
    dest->value = *code;
}

void decode_imm16(uint8_t* code, arg_t* dest)
{
    auto u16 = reinterpret_cast<uint16_t*>(code);
    dest->type = argument_type_t::imm16;
    dest->value = *u16;
}

uint8_t* decode_modrm(uint8_t* code, arg_t* start, bool reg_first = false, argument_type_t reg_type = argument_type_t::reg16, argument_type_t rm_type = argument_type_t::reg16)
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

    case 0b01:
        start->type = argument_type_t::displacement8;
        start->value = modrm.rm;
        start++;
        start->type = argument_type_t::displacement8;
        start->value = *code++;
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

    return code;
}

void decode_far_ptr(uint8_t* code, arg_t* dest)
{
    auto u16 = reinterpret_cast<uint16_t*>(code);
    auto extra = u16[0];
    auto seg = u16[1];

    dest->type = argument_type_t::pointer;
    dest->value = (seg << 4) + extra;
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
    case 0x31:  result.opcode = opcode_t::_xor; instruction = decode_modrm(instruction, &(result.args[0])); break;
    case 0x89:  result.opcode = opcode_t::mov; instruction = decode_modrm(instruction, &(result.args[0]));  break;
    case 0x8D:  result.opcode = opcode_t::lea; instruction = decode_modrm(instruction, &(result.args[0]), true); break;
    case 0x8E:  result.opcode = opcode_t::mov; instruction = decode_modrm(instruction, &(result.args[0]), true, argument_type_t::sreg16);  break;
    case 0xA5:  result.opcode = opcode_t::movs; break;
    case 0xB8:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 0; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xB9:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 1; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBA:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 2; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBB:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 3; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBC:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 4; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBD:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 5; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBE:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 6; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xBF:  result.opcode = opcode_t::mov; result.args[0].type = argument_type_t::reg16; result.args[0].value = 7; decode_imm16(instruction, &(result.args[1])); instruction += 2; break;
    case 0xEA:  result.opcode = opcode_t::jmp; decode_far_ptr(instruction, &(result.args[0])); instruction += 4; break;
    case 0xEB:  result.opcode = opcode_t::jmp; decode_rel8(instruction++, &(result.args[0]));   break;
    case 0xF3:  result.opcode = opcode_t::rep;                                                  break;
    case 0xFA:  result.opcode = opcode_t::cli;                                                  break;
    case 0xFC:  result.opcode = opcode_t::cld;                                                  break;
    default:    log_hex(opcode, "unknown opcode: ");                                            break;
    }

    result.length = static_cast<uint8_t>(instruction - instruction_start);

    return result;
}

instruction_t decode_current_instruction(core_ctx_t* core)
{
    auto phys = guest_memory_translate(core->rip);
    return decode_instruction(reinterpret_cast<uint8_t*>(phys), core->rip);
}
