char * const VIDMEM = (char *) 0xb8000;
static int char_ptr = 0;

void putc(char c)
{
    VIDMEM[char_ptr * 2] = c;
    char_ptr++;
    if (char_ptr >= 80 * 25) {
        char_ptr = 0;
    }
}

void puts(const char *s)
{
    while (*s != '\0') {
        putc(*s);
        s++;
    }
}

void clear(void)
{
    int i;
    for (i = 0; i < 80 * 25; i++) {
        VIDMEM[i * 2] = '\0';
    }
}

void kmain(void)
{
    clear();
    puts("Hello, i386 world!");
}
