--retain=g_pfnVectors /* Retain interrupt vector table */

#define APP_BASE 0x00004000 /* This has been modified as this app works with BL. */
#define RAM_BASE 0x20000000

/* System memory map */

MEMORY
{
    /* Application stored in and executes from internal flash */
    FLASH (RX) : origin = APP_BASE, length = 0x0001E000     /* This has been modified for the app to allocate just 120KB.*/
    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = 0x20000000, length = 0x00008000 /* 32KB */
}

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > APP_BASE          /* Interrupt vector table at start of FLASH */

    .text       : > FLASH           /* Code */
    .const      : > FLASH           /* Constants */
    .cinit      : > FLASH           /* Initialization data */
    .pinit      : > FLASH           /* Constructor lists */
    .init_array : > FLASH           /* C++ initializers */

    .vtable     : > RAM_BASE        /* Optional RAM vector table */
    .data       :   > SRAM          /* .data section (initialized variables) */
    .bss        :   > SRAM          /* .bss section (uninitialized variables) */

    .sysmem     : > SRAM            /* Dynamic memory (heap) */
    .stack      : > SRAM (HIGH)     /* Stack at end of SRAM */
}

/* Stack configuration */
__STACK_TOP = __stack + 512;
