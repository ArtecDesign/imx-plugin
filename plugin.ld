ENTRY(plugin_download)

MEMORY
{
  RAM (rwx)       : ORIGIN = 0x00910000, LENGTH = 64K
}

SECTIONS
{
  .flash_header :
  {
    _plugin_start = .;
    KEEP(*(.flash_header))
  } >RAM

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
  } >RAM

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
  } >RAM

  .data : 
  {
    . = ALIGN(4);
    *(.data)
    *(.data*)
  } >RAM

  .bss :
  {
	. = ALIGN(4);
    *(.bss)
    *(.bss*)
  } >RAM

  /* The size of plugin must be aligned to 512 bytes or the loader will corrupt data! */
  .align :
  {
	. = ALIGN(512);
	. += 511;
	BYTE(0x00);
  } >RAM
  
  _plugin_end = .;
  _plugin_size = _plugin_end - _plugin_start;
}
