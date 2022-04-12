<script>
    export let name = ''
    export let children = []
</script>

<div class="topbar-subitem">
    {name}
    {#if children.length}
        >
        <div class="topbar-subitem-children">
            {#each children as child}
                <svelte:self name={child.name} children={child.children} />
            {/each}
        </div>
    {/if}
</div>

<style>
    .topbar-subitem { position: relative; background-color: black; padding: 2px 8px; white-space: nowrap; cursor: pointer; }
    .topbar-subitem:hover > .topbar-subitem-children { display: flex; }
    .topbar-subitem-children { display: none; position: absolute; left: calc(100%); top: -1px; border: 1px white dotted; flex-direction: column; background-color: black;}
</style>