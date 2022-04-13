import '@/styles/index.css'
import Application from '@/templates/Application.svelte'
import { log_string } from '@/frontend/services/logger'
import { init_runtime, get_wasm_context } from './services/wasm'

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

    const ctx = get_wasm_context()
    let alloc = true
    let index = 0
    let count = 8
    let bases = []
    const malloc = ctx.runtime_instance.exports.__imp_malloc
    const free = ctx.runtime_instance.exports.__imp_free
    // setInterval(() =>
    // {
    //     if (index < count)
    //     {
    //         if (alloc)
    //         {
    //             bases.push(malloc(0x1000 * index));
    //         }
    //         else
    //         {
    //             free(bases.pop());
    //         }
    //         index++
    //     }
    //     else
    //     {
    //         index = 0
    //         alloc = !alloc
    //     }
    // }, 200);
    console.log(ctx.runtime_instance.exports)
}

window.addEventListener('load', main)
