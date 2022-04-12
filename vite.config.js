import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'
import markdown from 'vite-plugin-markdown'

export default defineConfig({
    server: {
        host: '0.0.0.0'
    },
    plugins: [
        svelte(),
        markdown({
            mode: 'html'
        })
    ],
    resolve: {
        alias: {
            '@': __dirname
        }
    }
})
