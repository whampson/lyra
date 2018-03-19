SECTIONS
{
    . = 0x7C00;
    .text :
    {
        *(.*)
        . = 0x1FE;
        SHORT(0xAA55)

        /* END OF BOOTSECTOR */

        QUAD(0x0000495A41475546)    /* "FUGAZI" */
    }
}
