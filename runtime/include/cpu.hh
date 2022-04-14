#pragma once

#include <macros.hh>
#include <types.hh>
#include <runtime.hh>

cpu_ctx_t* cpu_get_context();
void cpu_init();
void cpu_init_core(uint8_t index);

void cpu_core_set_rip(uint8_t core, uint64_t value);
