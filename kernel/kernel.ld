OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kmain)

SECTIONS
{
    .text 0x400000 :
        AT ( 0x00 )
    {
        LONG(kmain);
        *(.text)
    }
}
