#include <decoder.hh>
#include <cpu.hh>
#include <guest_memory.hh>
#include <imports.hh>

void decode_rel8(uint8_t* code, arg_t* dest)
{
    dest->type = argument_type_t::rel8;
    dest->value = *code;
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
    case 0xEB:  result.opcode = opcode_t::jmp; decode_rel8(instruction++, &(result.args[0]));   break;
    case 0xFA:  result.opcode = opcode_t::cli;                                                  break;
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
