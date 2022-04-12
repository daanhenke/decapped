<script>
import LogView from "./log/LogView.svelte";
import Window from "./misc/Window.svelte";
import TopBar from "./topbar/TopBar.svelte";

import { html as readme } from '@/markdown/readme.md'

const topbar_items = [
    { name: 'Hello', children: [ { name: 'World', children: [ { name: 'Test', children: [ { name: 'Test 2' }, { name: 'Test 3' } ] } ] } ] },
    { name: 'World' }
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
            <Window grow title="Instructions">
                0x0011223344556677: mov rax, rcx
            </Window>
        </div>
    </div>
</div>

<style>
    .container { max-height: 100vh; display: flex; flex-direction: column; }
    .content { padding: 2px 4px; display: flex; flex-direction: column; flex: 1 1 auto; }
    .instructions-container { display: flex; max-height: 15%; }
</style>
