import { log_string } from './logger'
import { copy_bytes_to, get_wasm_context } from './wasm'

interface floppy_controller_context
{
    floppies: ArrayBuffer[]
}

const ctx: floppy_controller_context = { floppies: [] }

export async function floppy_load_http(index: number, url: string)
{
    log_string(`loading floppy: '${url}' into drive #${index}\n`)
    const response = await fetch(url)
    const buffer = await response.arrayBuffer()
    ctx.floppies[index] = buffer
}

export function floppy_copy_sectors(index: number, first_sector: number, count: number, guest_address: number)
{
    const floppy = ctx.floppies[index]
    console.log(ctx)
    if (floppy === undefined) return
    console.log(ctx)

    const wasm_ctx = get_wasm_context()
    for (let i = 1; i < count; i++)
    {
        wasm_ctx.runtime_instance.exports.__imp_guest_memory_translate(guest_address + 512 * i)
    }
    const byte_size = count * 512
    const sector_data = floppy.slice(first_sector * 512, first_sector * 512 + byte_size)
    const host_address = wasm_ctx.runtime_instance.exports.__imp_guest_memory_translate(guest_address)
    console.log(`copying sectors ${first_sector}-${first_sector + count} to (${guest_address.toString(16)} > ${host_address.toString(16)})`)
    copy_bytes_to(host_address, sector_data)
}
