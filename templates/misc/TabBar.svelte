<script>
    export let content = []
    export let defaultTab = ''

    let currentTab = null
    if (defaultTab !== '')
    {
        for (let index in content)
        {
            const item = content[index]
            if (item.name === defaultTab)
            {
                currentTab = index
                break
            }
        }

        if (currentTab === null)
        {
            defaultTab = ''
        }
    }

    const is_active_tab = name => currentTab !== null && content[currentTab].name === name

    const on_tab_click = name =>
    {
        if (is_active_tab(name)) currentTab = null
        else
        {
            for (let index in content)
            {
                if (content[index].name === name)
                {
                    currentTab = index
                    break
                }
            }
        }
    }
</script>

<div class="tabbar">
    <div class="tabbar-items">
        {#each content as item}
            <div on:click={on_tab_click.bind(this, item.name)} class="tabbar-item { (currentTab !== null && content[currentTab].name === item.name) ? 'active' : '' }">
                { item.name }
            </div>
        {/each}
    </div>
    {#if currentTab !== null}
        <svelte:component this={content[currentTab].component} data={content[currentTab].data} />
    {/if}
</div>

<style>
    .tabbar { display: block; }
    .tabbar-items { display: inline-block; border-bottom: 1px white dotted; }
    .tabbar-item { display: inline; cursor: pointer; padding: 0 8px; }
    .tabbar-item.active { color: black; background-color: white; }
</style>
