<script>
    import Window from '../misc/Window.svelte'
    import { add_log_callback, get_context } from '@/frontend/services/logger'

    let messages = [ '' ]
    let log_container = null

    const on_update = (ctx) =>
    {
        messages = ctx.messages

        if (log_container !== null)
        {
            //console.log(log_container.lastChild)
        }
    }

    on_update(get_context())
    add_log_callback(on_update)
</script>

<Window grow title="Internal Logs">
    <div bind:this={log_container} class="log-container">
        {#each messages as message}
            <div class="log-message">
                { message }
            </div>
        {/each}
    </div>
</Window>

<style>
    @keyframes blink {
        from { color: white }
        to { color: black; }
    }
    .log-container { display: flex; flex-direction: column; height: 50%; }
    .log-message { display: flex; }
    .log-message:last-child:after { content: '_'; padding-left: 2px; animation: blink 1s; animation-iteration-count: infinite; }
</style>
