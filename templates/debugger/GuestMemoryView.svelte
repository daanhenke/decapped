<script>
    import Window from '@/templates/misc/Window.svelte'
    import { bytes_at, get_wasm_context } from '@/frontend/services/wasm'
    import PageTableView from './PageTableView.svelte';

    let tables = {
        p0: [],
        p1: [],
        p2: []
    }

    let tables_draft;
    const table_keys = [ 'p0', 'p1', 'p2' ]
    const pte_size = 4;
    const read_pt = (base, size, depth, parent = null) =>
    {
        const view = new DataView(bytes_at(base, size * pte_size))

        for (let i = 0; i < size; i++)
        {
            const child = view.getUint32(i * pte_size, true);
            if (child === 0) continue;

            const entry = { index: i, child, parent }
            const new_i = tables_draft[table_keys[depth]].push(entry) - 1
            if (depth < 2)
            {
                read_pt(child, 512, depth + 1, new_i)
            }
        }
    }

    const ctx = get_wasm_context()
    setInterval(() => {
        tables_draft = {
            p0: [],
            p1: [],
            p2: []
        }

        read_pt(ctx.runtime_instance.exports.guest_memory_get_p0(), 4, 0)

        tables = tables_draft
    }, 500)

    const reconstruct_addr = p2e =>
    {
        const p2i = p2e.index
        const p1e = tables.p1[p2e.parent]
        const p1i = p1e.index
        const p0e = tables.p0[p1e.parent]
        const p0i = p0e.index

        return ((p0i << 30) + (p1i << 21) + (p2i << 12)) >>> 0;
    }
</script>

<div>
    <PageTableView name="Level 0" table={tables.p0} />
    <PageTableView name="Level 1" table={tables.p1} parents={[tables.p0]} />
    <PageTableView name="Level 2" table={tables.p2} parents={[tables.p1, tables.p0]} />
    <Window title="Address Space">
        {#each tables.p2 as p2e}
            <div>{reconstruct_addr(p2e).toString(16)} > { p2e.child.toString(16) }</div>
        {/each}
    </Window>
</div>
