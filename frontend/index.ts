import '@/styles/index.css'
import Application from '@/templates/Application.svelte'
import { log_string } from '@/frontend/services/logger'
import { init_runtime, get_wasm_context, bytes_at } from './services/wasm'
import { floppy_copy_sectors, floppy_load_http } from './services/floppy'

const log_task = async (task: Promise<any>, message: string) =>
{
    log_string(`${message}...\n`)
    await task;
    log_string(`${message} done\n`)
}

const wait_async = (time: number) =>
{
    return new Promise(resolve => {
        setTimeout(() => resolve(0), time)
    })
}

const init_frontend = async () =>
{
    const app = new Application({
        target: document.body
    })
}

const main = async () =>
{
    await log_task(init_frontend(), "initializing frontend");
    await log_task(init_runtime(), "initializing runtime & backend");

    console.log('runtime exports:', ctx.runtime_instance.exports)

    const floppy_base = 0x20304050
    await floppy_load_http(0, 'floppies/x86BOOT.img')
    floppy_copy_sectors(0, 0, 17, floppy_base) // TODO: CHANGE TO 1 SECTOR
    ctx.runtime_instance.exports.__imp_cpu_core_set_rip(0, BigInt(floppy_base))
    ctx.runtime_instance.exports.__imp_log_instruction(floppy_base)
}

window.addEventListener('load', main)
