#include <disassembler.hh>
#include <macros.hh>
#include <decoder.hh>
#include <string.hh>
#include <memory.hh>
#include <imports.hh>
#include <guest_memory.hh>

void disassemble_instruction(instruction_t instruction, char* output)
{
    if (instruction.opcode == opcode_t::unknown) return;

    ultoa(instruction.address, output, 16);
    output = strcat(output, ": ");
    switch (instruction.opcode)
    {
        case opcode_t::unknown:
            return;

        case opcode_t::jmp: output = strcat(output, "jmp"); break;
        case opcode_t::cli: output = strcat(output, "cli"); break;
    }

    if (instruction.args[0].type == argument_type_t::none) return;
    output = strcat(output, " ");

    auto arg_i = 0;
    while (instruction.args[arg_i].type != argument_type_t::none)
    {
        auto& arg = instruction.args[arg_i];
        switch (arg.type)
        {
        case argument_type_t::rel8:
            auto dest = instruction.address + instruction.length + arg.value;
            ultoa(dest, output, 16);
            break;
        }
        arg_i++;
    }
}

export_func void __imp_disassemble_current_instruction(uint8_t core_index, char* output)
{
    auto instruction = decode_current_instruction(core_index);
    disassemble_instruction(instruction, output);
}

void log_instruction(instruction_t instruction)
{
    char* dest = malloc<char>(1024);

    disassemble_instruction(instruction, dest);
    log_string(dest);
    log_string("\n");

    free(dest);
}

export_func void __imp_log_instruction2(uintptr_t guest_address)
{
    auto phys = guest_memory_translate(guest_address);
    auto instruction = decode_instruction(reinterpret_cast<uint8_t*>(phys), guest_address);
    log_instruction(instruction);
}
