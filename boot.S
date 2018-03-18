.code16
.section .text

.globl _start
_start:
    movb    $0x0e, %ah
    leaw    hello, %bx

print_loop:
    movb    (%bx), %al
    cmpb    $0, %al
    je      halt
    int     $0x10
    incw    %bx
    jmp print_loop

halt:
    hlt
    jmp     halt

hello:
    .ascii "Hello, world!"
    .byte 0
