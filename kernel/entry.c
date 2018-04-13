void kmain(void) {
    char *vidmem = (char *) 0xb8000;
    vidmem[0] = 'a';
}
