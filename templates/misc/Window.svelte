<script>
    export let title = ''
    export let grow = false
    let minimized = false
    let closed = false

    const onToggleMinimize = () =>
    {
        minimized = !minimized
    }
</script>

<div class="window {grow ? 'window-grow' : ''}">
    <div on:click={onToggleMinimize} class="window-titlebar">
        <div>{title}</div>
        <div class="window-icons">
            <span>-</span>
            <span>x</span>
        </div>
    </div>

    {#if !minimized}
        <div class="window-content"><slot /></div>
    {/if}
</div>

<style>
    .window { margin: 8px 0; padding: 2px; border: 1px white solid; display: flex; flex-direction: column; }
    .window-grow { flex: 1 1 auto; }
    .window-titlebar { display: flex; justify-content: space-between; padding: 0 4px; }
    .window-icons { cursor: pointer; }
    .window-content { border-top: 1px white dotted;  padding: 5px; flex-grow: 1; overflow-y: auto; }
</style>
