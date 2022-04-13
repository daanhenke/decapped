import { log_string } from './logger'

interface wasm_context
{
    memory: WebAssembly.Memory|null
    runtime_imports: any
    runtime_instance: WebAssembly.Instance|null,
    interpreter: WebAssembly.Instance|null
}

const ctx: wasm_context = { memory: null, runtime_imports: {}, runtime_instance: null, interpreter: null }

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

    const import_section_start = type_section_start + type_section_size + 2
    const import_section_size = read_u32(view, import_section_start + 2)

    return patched_copy
}

export function bytes_at(offset: number, count: number = 8): ArrayBuffer
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

function __memset(ptr, value, num)
{
    const view = new DataView(bytes_at(ptr, num))
    for (let i = 0; i < num; i++)
    {
        view.setUint8(i, value);
    }

    return ptr;
}

function __log_string(ptr: number)
{
    log_string(`runtime: ${string_at(ptr)}`)
}

function __log_hex(number: number, prefix: number)
{
    if (prefix !== 0)
    {
        __log_string(prefix)
        log_string("");
    }
    log_string(`${number.toString(16)}\n`)
}

interface allocator_info
{
    pages_max: number
    pages_used: number,
    bitmap_base: number,
    size_base: number,
    current_index: number,
    dynamic_base: number,
    type_base: number
}

export function get_allocator_info(): allocator_info
{
    const base = ctx.runtime_instance.exports.malloc_get_dynamic_base()
    const view = new DataView(bytes_at(base, 0x20))

    return {
        pages_max:          view.getUint32(0x00, true),
        pages_used:         view.getUint32(0x04, true),
        bitmap_base:        view.getUint32(0x08, true),
        size_base:          view.getUint32(0x0C, true),
        current_index:      view.getUint32(0x10, true),
        dynamic_base:       view.getUint32(0x14, true),
        type_base:          view.getUint32(0x18, true),
    }
}

export function get_wasm_context(): wasm_context
{
    return ctx
}

export async function init_runtime()
{
    ctx.runtime_imports.memset = __memset
    ctx.runtime_imports.log_string = __log_string
    ctx.runtime_imports.log_hex = __log_hex
    const imports = ctx.runtime_imports

    ctx.memory = new WebAssembly.Memory({ initial: 10, maximum: 10, shared: true })
    log_string(`allocated memory pool, size: 0x${ctx.memory.buffer.byteLength.toString(16)}\n`)

    const runtime_module = await load_wasm_module('wasm/runtime.wasm')
    ctx.runtime_instance = new WebAssembly.Instance(runtime_module, {
        env: {
            memory: ctx.memory,
            ...imports
        },
    })

    const interpreter_module = await load_wasm_module('wasm/interpreter.wasm')
    ctx.interpreter = new WebAssembly.Instance(interpreter_module, {
        env: {
            memory: ctx.memory,
            ...imports
        }
    })


    log_string('calling runtime_init\n')
    const runtime_result = ctx.runtime_instance.exports.runtime_init()
    const malloc_base = ctx.runtime_instance.exports.malloc_get_dynamic_base();
    log_string(`malloc start: ${malloc_base.toString(16)}\n`)
    log_string(`runtime_init finished, result: 0x${runtime_result.toString(16)}\n`)

log_string('calling interpreter_init\n')
    const interpreter_result = ctx.interpreter.exports.interpreter_init()
    log_string(`interpreter_init finished, result: 0x${interpreter_result.toString(16)}\n`)

    window.ctx = ctx
}
