<script>
    import Window from '@/templates/misc/Window.svelte'

    export let name = ''
    export let parents = []
    export let table = []

    const get_full_index = entry =>
    {
        const indexes = [ entry.index ]

        let parent_i = 0
        while (entry.parent !== null && parent_i < parents.length)
        {
            entry = parents[parent_i][entry.parent]
            indexes.push(entry.index)
            parent_i++
        }

        return indexes.map(i => i.toString(16)).join(' ')
    }
</script>

<Window title={name}>
    {#each table as entry}
        <div>
            { get_full_index(entry) } - { entry.child.toString(16) }
        </div>
    {/each}
</Window>
