ENTRY(reset)

MEMORY
{
rom       : ORIGIN = 0x00000000, LENGTH = 0x00400000 /* 4 mio */
ram       : ORIGIN = 0x00400000, LENGTH = 0x02000000 /* 32 mio */
stack     : ORIGIN = 0x02400000, LENGTH = 0x00400000 /* 4 mio */
uart      : ORIGIN = 0x02800000, LENGTH = 0x00100000 /* 1 mio */
}

SECTIONS
{
.reset : { *startup.o(.text) } > rom
.main : { *main.o(.text) } > rom
.text : { *(.text) } > rom
.data : { *(.data) } > rom
.rodata : { *(.data) } > rom
.bss : { *(.bss) } > rom

ld_heap_low  = ORIGIN(ram);
ld_heap_top  = ORIGIN(ram) + LENGTH(ram);
ld_stack_top = ORIGIN(stack) + LENGTH(stack);
uart_base    = ORIGIN(uart);
}

