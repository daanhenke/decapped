#include <memory.hh>
#include <guest_memory.hh>

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

    return 0x1337;
}
