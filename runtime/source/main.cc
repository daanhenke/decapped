#include <macros.hh>
#include <memory.hh>
#include <imports.hh>

export_func int runtime_init()
{
    log_string("hello world\n");
    malloc_init();

    for (auto i = 0; i < 20; i++)
    {
        log_string("something\n");
    }

    return 0x1337;
}
