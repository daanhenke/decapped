type logger_callback = (ctx: logger_context) => void;

interface logger_context
{
    messages: string[]
    callbacks: logger_callback[]
}

const ctx: logger_context = { messages: [''], callbacks: [] }

export function log_string(message: string)
{
    const lines = message.split('\n')

    for (let index in lines)
    {
        const line = lines[index]
        ctx.messages[ctx.messages.length - 1] += line
        if (index < lines.length - 1) ctx.messages.push('')
    }

    for (let cb of ctx.callbacks) cb(ctx)
}

export function get_context(): logger_context
{
    return ctx
}

export function add_log_callback(cb: logger_callback)
{
    ctx.callbacks.push(cb)
}