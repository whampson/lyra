void fill_screen(char ch)
{
    char * const VGA_BUF = (char *) 0xb8000;

    int i;
    for (i = 0; i < 25 * 80; i++)
    {
        VGA_BUF[2 * i] = ch;
        VGA_BUF[2 * i + 1] = 0x07;
    }
}

void kernel_main(void)
{
    char ch = '\0';

    fill_screen(ch);
}
