#include <memory.hh>
#include <guest_memory.hh>
#include <emulator.hh>

#include <macros.hh>
#include <imports.hh>
#include <types.hh>

export_func status_t runtime_init()
{
    log_string("hello world\n");
    malloc_init();
    if (! guest_memory_init())
    {
        log_string("failed to init guest_memory!\n");
        return 1;
    }
    emulator_init();

    log_string("accessing some guest memory at 0xDEADBEEF");
    for (auto i = 0; i < 30; i++)
    {
        guest_memory_translate(0xDEADBEEF + i * 0x1000);
    }
    return 0x1337;
}
