SECTIONS
{
    . = 0x7C00;
    .stage1 :
    {
        *(.stage1)
        . = 0x1FE;
        SHORT(0xAA55)
    }

    . = 0x7E00;
    .stage2 :
    {
        *(.stage2)
    }
}
