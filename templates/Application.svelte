<script>
import LogView from "./log/LogView.svelte";
import Window from "./misc/Window.svelte";
import TopBar from "./topbar/TopBar.svelte";

import { html as readme } from '@/markdown/readme.md'
import Debugger from "./debugger/Debugger.svelte";

const change_zoom = (scale = 1) =>
{
    document.body.style.zoom = `${scale * 100}%`
}

const mkcb = (cb, ...const_args) => (...args) => cb(...const_args, ...args)

const topbar_items = [
    { name: 'View', children: [
        { name: 'Zoom', children: [
            { name: '100%', click: mkcb(change_zoom, 1) },
            { name: '200%', click: mkcb(change_zoom, 2) },
            { name: '300%', click: mkcb(change_zoom, 3) },
            { name: '400%', click: mkcb(change_zoom, 4) },
        ] }
    ] }
]

</script>
<div class="container">
    <TopBar items={topbar_items} />
    <div class="content">
        <Window title="README">
            <div class="markdown">{@html readme}</div>
        </Window>
        <LogView />
        <div class="instructions-container">
            <Debugger />
        </div>
    </div>
</div>

<style>
    .container { max-height: 100vh; display: flex; flex-direction: column; }
    .content { padding: 2px 4px; display: flex; flex-direction: column; flex: 1 1 auto; }
    .instructions-container { display: flex; flex-direction: column; max-height: 15%; }
</style>
