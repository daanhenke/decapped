<script>
    export let data;
</script>

<div>
    {#if data}
        {#each data.registers as register}
            <div>{register.name}: {register.value.toString(16)}</div>
        {/each}
    {/if}
</div>
