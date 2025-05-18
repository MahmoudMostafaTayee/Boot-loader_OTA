--retain=g_pfnVectors /* Retain interrupt vector table */

#define APP_BASE 0x00000000 /* Bootloader starts at 0x00000000 */
#define RAM_BASE 0x20000000

/* System memory map */

MEMORY
{
    /* Application stored in and executes from internal flash */
    FLASH (RX) : origin = APP_BASE, length = 0x00004000     /* This has been modified to be 0x00004000 instead of whole Flash(0x00040000) as the BL length is set to 16KB.*/
    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = 0x20000000, length = 0x00008000   /* 32KB */
}

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > APP_BASE       /* Interrupt vector table at start of FLASH */

    .text       : > FLASH        /* Code */
    .const      : > FLASH        /* Constants */
    .cinit      : > FLASH        /* Initialization data */
    .pinit      : > FLASH        /* Constructor lists */
    .init_array : > FLASH        /* C++ initializers */

    .vtable     : > RAM_BASE     /* Optional RAM vector table */

    /* .data section (initialized variables) */
    .data: 
    { 
        __BL_DATA_START = .;    /* Start of .data in SRAM */
        *(.data)                /* All .data sections */
        __BL_DATA_END = .;      /* End of .data */
    } > SRAM

    /* .bss section (uninitialized variables) */
    .bss: 
    { 
        __BL_BSS_START = .;     /* Start of .bss */
        *(.bss)                 /* All .bss sections */
        *(COMMON)               /* COMMON section (e.g., globals) */
        __BL_BSS_END = .;       /* End of .bss */
    } > SRAM

    .sysmem     : > SRAM         /* Dynamic memory (heap) */
    .stack      : > SRAM (HIGH)  /* Stack at end of SRAM */
}
/* Explicitly retain symbols to prevent optimization */
--retain="__BL_DATA_START"
--retain="__BL_DATA_END"
--retain="__BL_BSS_START"
--retain="__BL_BSS_END"

/* Stack configuration */
__STACK_TOP = __stack + 512;
