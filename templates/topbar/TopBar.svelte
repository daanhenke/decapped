<script>
import SubItem from "./SubItem.svelte";

    export let items = []
</script>

<div class="topbar">
    {#each items as item}
        <div class="topbar-item">
            <div class="topbar-item-name">
                {item.name}
            </div>
            {#if item.children}
            <div class="topbar-item-children">
                {#each item.children as child}
                    <SubItem name={child.name} children={child.children} click={child.click} />
                {/each}
            </div>
            {/if}
        </div>
    {/each}
</div>

<style>
    .topbar { display: flex; border-bottom: 1px white dotted; }
    .topbar-item { position: relative; }
    .topbar-item-name { padding: 2px 8px; cursor: pointer; }
    .topbar-item:hover .topbar-item-children { display: flex; }
    .topbar-item-children:hover { display: flex; }
    .topbar-item-children { display: none; position: absolute; top: 100%; left: 0; border: 1px white dotted; }
</style>
