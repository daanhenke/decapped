#include <disassembler.hh>
#include <macros.hh>
#include <decoder.hh>

export_func void __imp_disassemble_current_instruction(uint8_t core_index, char* output)
{
    decode_current_instruction(core_index);
}
