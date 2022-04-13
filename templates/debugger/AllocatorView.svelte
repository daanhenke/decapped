<script>
    import Window from '@/templates/misc/Window.svelte'
    import { get_allocator_info, bytes_at } from '@/frontend/services/wasm'

    let allocator_info_list = [ { name: 'Loading', value: '...' } ]
    let allocation_list = {}

    const name_lookup = {
        pages_max: 'Max pages',
        pages_used: 'Pages used',
        bitmap_base: 'Bitmap',
        size_base: 'Size Array',
        current_index: 'Head',
        dynamic_base: 'Dynamic Base',
        type_base: 'Type Base'
    }

    const _hex = x => x.toString(16)
    const _string = x => x.toString()
    const format_lookup = {
        pages_max: _hex,
        pages_used: _hex,
        bitmap_base: _hex,
        size_base: _hex,
        type_base: _hex,
        dynamic_base: _hex,
        current_index: _hex,
    }
    const types = [ 'generic', 'runtime', 'page table', 'guest memory' ]

    const update_allocation_list = (allocator_info) =>
    {
        const result = {}
        const bitmap_size = allocator_info.pages_max / 8;
        const bitmap_view = new DataView(bytes_at(allocator_info.bitmap_base, bitmap_size))
        const size_view = new DataView(bytes_at(allocator_info.size_base, allocator_info.pages_max * 4))
        const type_view = new DataView(bytes_at(allocator_info.type_base, allocator_info.pages_max))


        let i = 0;
        while (i < allocator_info.pages_max && i < 0x100)
        {
            const byte = bitmap_view.getUint8(Math.floor(i / 8));
            if (byte & (1 << (i % 8)))
            {
                const base = allocator_info.dynamic_base + i * 0x1000;
                const at = allocation_list.hasOwnProperty(base) ? allocation_list[base].at : Math.floor(new Date().getTime() / 1000);
                const size = size_view.getUint32(i * 4, true)
                const type = type_view.getUint8(i);
                console.log(type, type_view, size_view, i, allocator_info.type_base);
                result[base] = { at, size, type }
                i += size > 0 ? size : 1;
            }
            else
            {
                i++;
            }
        }

        allocation_list = result
    }

    setInterval(() =>
    {
        const result = []
        const allocator_info = get_allocator_info()
        for (let key in allocator_info)
        {
            const name = name_lookup[key]
            result.push({ name, value: format_lookup[key](allocator_info[key]) })
        }

        allocator_info_list = result
        update_allocation_list(allocator_info)
    }, 500)
</script>

<div>
    {#each allocator_info_list as info_entry}
        <div class="allocator-info-entry">
            { info_entry.name }: {info_entry.value}
        </div>
    {/each}
    <Window title="Allocations">
        {#each Object.keys(allocation_list) as base}
            <pre>[{types[allocation_list[base].type]}]&#9;0x{ parseInt(base).toString(16) }&#9; - 0x{ allocation_list[base].size.toString(16) }&#9;@ { allocation_list[base].at }</pre>
        {/each}
    </Window>
</div>
