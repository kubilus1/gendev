OUTPUT_ARCH(sh)
SEARCH_DIR(.)
__DYNAMIC  =  0;

/*
 * The memory map look like this:
 * +--------------------+ <- 0x02300000
 * | .text              |
 * |        _etext      |
 * |        ctor list   | the ctor and dtor lists are for
 * |        dtor list   | C++ support
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * +--------------------+ <- 0x06000000
 * | .data              | initialized data goes here
 * |        _edata      |
 * +--------------------+
 * | .bss               |
 * |        __bss_start | start of bss, cleared by crt0
 * |        _end        | start of heap, used by sbrk()
 * +--------------------+
 * .                    .
 * .                    .
 * .                    .
 * |        __stack     | top of stack (for Master SH2)
 * +--------------------+ <- 0x0603FC00
 */

MEMORY
{
    psram (wx) : ORIGIN = 0x02300000, LENGTH = 0x00100000
    ram (wx) : ORIGIN = 0x06000000, LENGTH = 0x0003FC00
}

/*
 * Allocate the stack to be at the top of memory, since the stack
 * grows down
 */

PROVIDE (__stack = 0x0603FC00);

SECTIONS
{
  .text 0x02300000 :
  AT ( 0x00000000 )
  {
    *(.text .text.*)
    . = ALIGN(0x4);
     __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    *(.dtors)
     LONG(0)
    __DTOR_END__ = .;
    *(.rodata .rodata.*)
    *(.gcc_except_table .gcc_except_table.*)

    . = ALIGN(0x10);
    __INIT_SECTION__ = . ;
    *(.init)
    SHORT (0x000B)	/* rts */
    SHORT (0x0009)	/* nop */

    . = ALIGN(0x10);
    __FINI_SECTION__ = . ;
    *(.fini)
    SHORT (0x000B)	/* rts */
    SHORT (0x0009)	/* nop */

    _etext = .;
    *(.lit)
    . = ALIGN(0x4);
  } > psram
  _stext = SIZEOF (.text);

  .data 0x06000000 :
  AT ( LOADADDR (.text) + SIZEOF (.text) )
  {
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.*)
    _edata = .;
    . = ALIGN(0x4);
  } > ram
  _sdata = SIZEOF (.data);

  .bss 0x06000000 + SIZEOF (.data) :
  {
    __bss_start = . ;
    *(.shbss)
    *(.bss .bss.*)
    *(COMMON)
    _end =  ALIGN (0x8);
    __end = _end;
    end = _end;
  } > ram

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }

  .eh_frame 0 (NOLOAD) :
  {
    *(.eh_frame)
  }
}
