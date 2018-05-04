#ifndef _KERNEL_H_
#define _KERNEL_H_

#define KERNEL_START        0x400000

void putc(char c);
void puts(const char *s);
void clear(void);

extern void kmain(void);
#endif