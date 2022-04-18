#include <disassembler.hh>
#include <macros.hh>
#include <decoder.hh>
#include <string.hh>
#include <memory.hh>
#include <imports.hh>
#include <guest_memory.hh>

const char* reg_names_16[] = {
    "ax",
    "cx",
    "dx",
    "bx",
    "sp",
    "bp",
    "si",
    "di"
};

const char* reg_names_8[] = {
    "al",
    "cl",
    "dl",
    "bl",
    "ah",
    "ch",
    "dh",
    "bh"
};

const char* sreg_names_16[] = {
    "es",
    "cs",
    "ss",
    "ds",
    "fs",
    "gs",
    "<ERROR>",
    "<ERROR>"
};

const char* displacement8_names[] = {
    "[bx + si]",
    "[bx + di]",
    "[bp + si]",
    "[bp + di]",
    "[si]",
    "[di]",
    "[bp]",
    "[bx]"
};

void disassemble_instruction(instruction_t instruction, char* output, bool prefix)
{
    if (instruction.opcode == opcode_t::unknown) return;

    if (prefix)
    {
        output = ultoa(instruction.address, output, 16);
        output = strcat(output, ": ");
    }
    switch (instruction.opcode)
    {
        case opcode_t::unknown:
        log_string("dissasembling unknown opcode!\n");
            return;

        case opcode_t::add: output = strcat(output, "add");     break;
        case opcode_t::adc: output = strcat(output, "adc");     break;
        case opcode_t::_xor: output = strcat(output, "xor");    break;
        case opcode_t::lea: output = strcat(output, "lea");     break;
        case opcode_t::mov: output = strcat(output, "mov");     break;
        case opcode_t::movs: output = strcat(output, "movs");   break;
        case opcode_t::jmp: output = strcat(output, "jmp");     break;
        case opcode_t::rep: output = strcat(output, "rep");     break;
        case opcode_t::cli: output = strcat(output, "cli");     break;
        case opcode_t::sti: output = strcat(output, "sti");     break;
        case opcode_t::cld: output = strcat(output, "cld");     break;
        case opcode_t::nop: output = strcat(output, "nop");     break;
        case opcode_t::cbw: output = strcat(output, "cbw");     break;
        case opcode_t::mul: output = strcat(output, "mul");     break;
        case opcode_t::shr: output = strcat(output, "shr");     break;
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
            output = ultoa(instruction.address + instruction.length + arg.value, output, 16);
            break;
        case argument_type_t::imm8:
        case argument_type_t::imm16:
            output = ultoa(arg.value, output, 16);
            break;
        case argument_type_t::reg8:
            output = strcat(output, reg_names_8[arg.value]);
            break;
        case argument_type_t::reg16:
            output = strcat(output, reg_names_16[arg.value]);
            break;
        case argument_type_t::sreg16:
            output = strcat(output, sreg_names_16[arg.value]);
            break;
        case argument_type_t::pointer:
            output = ultoa(arg.value, output, 16);
            break;
        case argument_type_t::displacement8:
            output = strcat(output, displacement8_names[arg.value]);
            arg_i++;
            if (static_cast<int8_t>(instruction.args[arg_i].value) >= 0)
            {
                output = strcat(output, "+");
            }
            output = itoa(instruction.args[arg_i].value, output, 16);
            break;
        }
        arg_i++;

        if (instruction.args[arg_i].type != argument_type_t::none)
        {
            output = strcat(output, ", ");
        }
    }
}

void log_instruction(instruction_t instruction, bool prefix)
{
    char* dest = malloc<char>(1024);
    memset(dest, 0, 1024);

    disassemble_instruction(instruction, dest, prefix);
    log_string(dest);

    free(dest);
}
