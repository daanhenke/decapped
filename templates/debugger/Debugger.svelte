<script>
    import Window from "@/templates/misc/Window.svelte";
    import TabBar from "../misc/TabBar.svelte";
    import AllocatorView from "./AllocatorView.svelte";
import GuestMemoryView from "./GuestMemoryView.svelte";

    const tab_bar_data = [
        { name: 'Allocator', component: AllocatorView },
        { name: 'Guest Memory', component: GuestMemoryView },
        { name: 'Registers', component: null }
    ]
</script>

<Window title="Debugger">
    <TabBar content={tab_bar_data} defaultTab={"Allocator"} />
</Window>
