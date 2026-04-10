ENTRY( _start )

__stack_size = 1024;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    RAM   (xrw): ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .init :
    {
        _sinit = .;
        . = ALIGN(4);
        KEEP(*(SORT_NONE(.init)))
        . = ALIGN(4);
        _einit = .;
    } >FLASH AT>FLASH

    .vector :
    {
        *(.vector);
        . = ALIGN(64);
    } >FLASH AT>FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH AT>FLASH

    .fini :
    {
        KEEP(*(SORT_NONE(.fini)))
        . = ALIGN(4);
    } >FLASH AT>FLASH

    PROVIDE( _etext = . );

    /* Data section - критично для _data_lma / _data_vma */
    .data :
    {
        PROVIDE( _data_lma = LOADADDR(.data) );   /* ← это было нужно */
        PROVIDE( _data_vma = . );
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        . = ALIGN(4);
        PROVIDE( _edata = . );
    } >RAM AT>FLASH

    .bss :
    {
        . = ALIGN(4);
        PROVIDE( _sbss = . );
        *(.sbss*)
        *(.bss*)
        *(COMMON*)
        . = ALIGN(4);
        PROVIDE( _ebss = . );
    } >RAM AT>FLASH

    PROVIDE( _end = _ebss );

    /* Стек */
    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        PROVIDE( _heap_end = . );
        . = ALIGN(4);
        PROVIDE( _susrstack = . );
        . = . + __stack_size;
        PROVIDE( _eusrstack = . );
    } >RAM
}
