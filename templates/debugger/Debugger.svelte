<script>
    import Window from "@/templates/misc/Window.svelte";
    import { get_allocator_info, get_wasm_context } from '@/frontend/services/wasm'

    let allocator_info_list = [ { name: 'Loading', value: '...' } ]

    const name_lookup = {
        pages_max: 'Max pages',
        pages_used: 'Pages used'
    }

    const _hex = x => x.toString(16)
    const _string = x => x.toString()
    const format_lookup = {
        pages_max: _hex,
        pages_used: _hex
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

        get_wasm_context().runtime_instance.exports.malloc(233);
    }, 500)
</script>

<Window title="Debugger">
    <h1>allocator</h1>
    <div class="allocator-info">
        {#each allocator_info_list as info_entry}
            <div class="allocator-info-entry">
                { info_entry.name }: {info_entry.value}
            </div>
        {/each}
    </div>
</Window>
