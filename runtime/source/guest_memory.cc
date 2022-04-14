#include <guest_memory.hh>
#include <memory.hh>
#include <imports.hh>

constexpr uint32_t pt_root_count = 4;
constexpr uint32_t pt_entry_count = 512;
guest_pagetable_entry_t* pt_root = nullptr;

void zero_page_table(guest_pagetable_entry_t* pt)
{
    for (auto i = 0; i < pt_entry_count; i++)
    {
        pt[i].dest = 0;
    }
}

export_func uintptr_t guest_memory_get_p0()
{
    return reinterpret_cast<uintptr_t>(pt_root);
}

uintptr_t guest_memory_translate(uintptr_t address)
{
    address_translation_t trans;
    trans.full = address;

    auto p0 = &pt_root[trans.p0];
    if (p0->dest == 0)
    {
        log_hex(trans.p0, "creating p1 for p0 ");
        auto new_p1 = malloc<guest_pagetable_entry_t>(pt_entry_count, allocation_type_t::page_table);
        p0->dest = reinterpret_cast<uintptr_t>(new_p1);
        zero_page_table(new_p1);
    }

    auto p1 = &(reinterpret_cast<guest_pagetable_entry_t*>(p0->dest)[trans.p1]);
    if (p1->dest == 0)
    {
        log_hex(trans.p1, "creating p2 for p1 ");
        auto new_p2 = malloc<guest_pagetable_entry_t>(pt_entry_count, allocation_type_t::page_table);
        p1->dest = reinterpret_cast<uintptr_t>(new_p2);
        zero_page_table(new_p2);
    }

    auto p2 = &(reinterpret_cast<guest_pagetable_entry_t*>(p1->dest)[trans.p2]);

    if (p2->dest == 0)
    {
        log_string("allocating guest page\n");
        p2->dest = reinterpret_cast<uintptr_t>(malloc(0x1000, allocation_type_t::guest_memory));
    }

    return p2->dest + trans.offset;
}

export_func uintptr_t __imp_guest_memory_translate(uintptr_t address)
{
    return guest_memory_translate(address);
}

bool guest_memory_init()
{
    log_string("initializing guest memory...\n");
    pt_root = malloc<guest_pagetable_entry_t>(pt_root_count, allocation_type_t::page_table);
    for (auto i = 0; i < pt_root_count; i++)
    {
        pt_root[i].dest = 0;
    }

    return true;
}
