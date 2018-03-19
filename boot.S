.code16
.section .text

STACK_BASE = 0x00007C00

/**
 * Bootloader entry point -- where it all begins!
 */
.globl entry
entry:
    movw    $STACK_BASE, %bp
    movw    %bp, %sp

    leaw    s_hello, %bx
    call    print

    call    read_sect
    movw    $0x7E00, %bx
    call    print

    jmp     halt

read_sect:
    movw    $0x07E0, %ax
    movw    $0x0000, %bx
    movw    %ax, %es
    movb    $0x02, %ah      /* func: read sectors */
    movb    $1, %al         /* num sectors */
    movb    $0, %ch         /* cylinder num */
    movb    $2, %cl         /* sector num */
    movb    $0, %dh         /* head num */
    movb    $0, %dl         /* drive num */
    int     $0x13
    jc      _read_sect_error
    ret

_read_sect_error:
    leaw    s_disk_err, %bx
    call    print
    jmp     halt


/**
 * Prints a zero-terminated string to the screen at the current cursor position.
 *
 *   Inputs: bx - address of string to print
 *  Outputs: (none)
 * Clobbers: ax
 */
print:
    movb    $0x0e, %ah      /* 0x0e - TTY mode */

_print_loop:
    movb    (%bx), %al
    cmpb    $0, %al
    je      _print_done
    int     $0x10
    incw    %bx
    jmp     _print_loop

_print_done:
    ret

halt:
    hlt
    jmp     halt

s_hello:
    .ascii "Hello, world!"
    .byte 10, 13, 0

s_disk_err:
    .ascii "Disk read error!"
    .byte 10, 13, 0
