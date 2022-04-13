<script>
    import { bytes_at, get_wasm_context } from '@/frontend/services/wasm'
import TabBar from '../misc/TabBar.svelte'
import CoreView from './CoreView.svelte';

    let core_count = 0
    let current_core = 0
    let tabs = []
    let default_tab = ''

    const update_info = () =>
    {
        const ctx = get_wasm_context()
        if (ctx.runtime_instance === null) return
        const cpu_ctx_base = ctx.runtime_instance.exports.emulator_get_context()
        const cpu_ctx_size = 8;
        const cpu_view = new DataView(bytes_at(cpu_ctx_base, cpu_ctx_size))
        core_count = cpu_view.getUint8(0)

        const core_ptr_base = cpu_view.getUint32(4, true)
        const core_ptr_size = 4
        const core_ptr_view = new DataView(bytes_at(core_ptr_base, core_ptr_size * core_count))

        const regs = [ 'rax', 'rbx', 'rcx', 'rdx' ,'rdi', 'rsi', 'rbp', 'rsp', 'rip', 'rflags' ]
        const new_tabs = []
        for (let i = 0; i < core_count; i++)
        {
            const core_base = core_ptr_view.getUint32(i * core_ptr_size, true)
            const core_size = 8 + 8 + 8 + 8 + 8 + 8 + 8 + 8 + 8 + 8;
            const core_view = new DataView(bytes_at(core_base, core_size))

            const core = {
                registers: []
            }

            for (let index in regs)
            {
                const name = regs[index]
                const value = core_view.getBigUint64(8 * index, true)
                core.registers.push({ name, value })
            }

            new_tabs.push({ name: `Core ${i}`, component: CoreView, data: core })
        }
        tabs = new_tabs
        default_tab = new_tabs[0].name
    }

    setInterval(update_info, 500)
    update_info()
</script>

<div>
    core count: {core_count}
    <TabBar content={tabs} defaultTab={default_tab} />
</div>
