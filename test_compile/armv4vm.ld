ENTRY(reset)

MEMORY
{
rom       : ORIGIN = 0x00000000, LENGTH = 0x00400000 /* 4 mio */
ram       : ORIGIN = 0x00400000, LENGTH = 0x04000000 /* 32 mio */
stack     : ORIGIN = 0x04400000, LENGTH = 0x00600000 /* 4 mio */
uart      : ORIGIN = 0x05000000, LENGTH = 0x00100000 /* 1 mio */
}

SECTIONS
{
.reset : { *startup.o(.text) } > rom
.text : { *(.text) } > rom
.data : { *(.data) } > rom
.bss : { *(.bss) } > rom

ld_heap_low  = ORIGIN(ram);
ld_heap_top  = ORIGIN(ram) + LENGTH(ram);
ld_stack_top = ORIGIN(stack) + LENGTH(stack);
uart_base    = ORIGIN(uart);
}

