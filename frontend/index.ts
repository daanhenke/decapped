import '@/styles/index.css'
import Application from '@/templates/Application.svelte'
import { log_string } from '@/frontend/services/logger'
import { init_runtime } from './services/wasm'

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
    await log_task(init_runtime(), "initializing runtime");
}

window.addEventListener('load', main)
