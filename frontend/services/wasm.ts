import { log_string } from './logger'

interface wasm_context
{
    memory: WebAssembly.Memory|null
    runtime_imports: any
    runtime_instance: WebAssembly.Instance|null
}

const ctx: wasm_context = { memory: null, runtime_imports: {}, runtime_instance: null }

async function load_wasm_module(url: string)
{
    const response = await fetch(url)
    const patched_module = patch_wasm_memory_import(await response.arrayBuffer(), 1, 2)
    return await WebAssembly.compile(patched_module)
}

function patch_wasm_memory_import(module: ArrayBuffer, initial_size = 1, max_size = 1): ArrayBuffer
{
    const read_u32 = (view: DataView, offset) =>
    {
        let result = 0
        let shift = 0
        let i = 0

        while (true)
        {
            console.log('it', i, shift, result, offset)
            const byte = view.getUint8(offset + i)
            result |= (byte & 0b01111111) << shift;
            if ((byte & 0xb10000000) === 0) break;
            shift += 7;
            i++;
        }

        return result
    }

    const patched_copy = module.slice(0, module.byteLength)
    const view = new DataView(patched_copy)

    const magic = view.getUint32(0)
    if (magic !== 0x0061736D)
    {
        return patched_copy
    }

    const type_section_start = 4 + 4
    const type_section_size = read_u32(view, type_section_start + 1)
    console.log(type_section_size)

    const import_section_start = type_section_start + type_section_size + 2
    const import_section_size = read_u32(view, import_section_start + 2)

    console.log(import_section_size)


    return patched_copy
}

function bytes_at(offset: number, count: number = 8): ArrayBuffer
{
    return ctx.memory.buffer.slice(offset, offset + count)
}

function string_at(offset: number)
{
    let result = ''

    let i = 0
    let done = false
    while (! done)
    {
        const u8 = new Uint8Array(bytes_at(offset + i))
        i += u8.byteLength

        for (let j = 0; j < u8.byteLength; j++)
        {
            if (u8[j] === 0)
            {
                done = true
                break
            }

            result += String.fromCharCode(u8[j])
        }
    }

    return result
}

function __log_string(ptr: number)
{
    log_string(`runtime: ${string_at(ptr)}`)
}

export async function init_runtime()
{
    const module = await load_wasm_module('/wasm/runtime.wasm')
    console.log(module)

    ctx.runtime_imports.log_string = __log_string
    const imports = ctx.runtime_imports

    ctx.memory = new WebAssembly.Memory({ initial: 10, maximum: 10, shared: true })
    log_string(`allocated memory pool, size: 0x${ctx.memory.buffer.byteLength.toString(16)}\n`)
    ctx.runtime_instance = new WebAssembly.Instance(module, {
        env: {
            memory: ctx.memory,
            ...imports
        },
    })

    log_string('calling runtime_init\n')
    const result = ctx.runtime_instance.exports.runtime_init()
    const malloc_base = ctx.runtime_instance.exports.malloc_get_dynamic_base();
    console.log(bytes_at(malloc_base, 4))
    log_string(`runtime_init finished, result: 0x${result.toString(16)}\n`)
}
