#pragma once

#include <types.hh>
#include <cpu.hh>

export_func const char* backend_get_name();
export_func status_t backend_init();
export_func status_t backend_tick(uint8_t core);
