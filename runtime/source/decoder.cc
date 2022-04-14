#include <decoder.hh>
#include <cpu.hh>
#include <guest_memory.hh>
#include <imports.hh>

void decode_instruction(char* instruction)
{
    log_string(const_cast<const char*>(instruction));
}

void decode_current_instruction(uint8_t core_index)
{
    auto cpu = cpu_get_context();
    auto core = cpu->cores[core_index];
    auto phys = guest_memory_translate(core->rip);
    return decode_instruction(reinterpret_cast<char*>(phys));
}
