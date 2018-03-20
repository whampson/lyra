/*------------------------------------------------------------------------------
 *
 * File: bootloader.ld
 * Desc: Linker script for the boot code.
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(entry)

STAGE1_BASE = 0x7C00;
STAGE2_BASE = 0x7E00;

SECTIONS
{
    .stage1 (STAGE1_BASE) :
    {
        *(.stage1)
        . = 0x1FE;
        SHORT(0xAA55)
    }

    .stage2 (STAGE2_BASE) :
    {
        *(.stage2)
    }
}
