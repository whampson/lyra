.code16
.section .stage1, "ax", @progbits

STACK_BASE  = 0x7C00
STAGE2_SEG  = 0x07E0
STAGE2_OFF  = 0x0000
SECT_COUNT  = 4
RETRY_COUNT = 3

##
# Bootloader entry point -- where it all begins!
##
.globl entry
entry:
    movw    $STACK_BASE, %bp
    movw    %bp, %sp

    call    load_stage2
    jmp     stage2

    jmp     halt

    # TODO: (bootloader)
    #       enable A20
    #       setup GDT
    #       enable prot. mode
    #       load kernel
    #           floppy disk driver
    #       jump to kernel entry point
    # TODO: (kernel)
    #       load IDT and other x86 structures
    #       enable paging

load_stage2:
    movw    $STAGE2_SEG, %ax
    movw    $STAGE2_OFF, %bx
    movw    %ax, %es
    movw    $RETRY_COUNT, %si

_stage2_loop:
    decw    %si
    movb    $0x02, %ah          # func: read floppy
    movb    $SECT_COUNT, %al    # number of sectors to read
    movb    $0, %ch             # cylinder number
    movb    $2, %cl             # sector number
    movb    $0, %dh             # head number
    movb    $0, %dl             # drive number
    int     $0x13
    jc      _stage2_retry
    ret

_stage2_retry:
    cmpw    $0, %si
    je      _stage2_error
    jmp     _stage2_loop

_stage2_error:
    leaw    s_disk_err, %bx
    call    print
    jmp     halt

s_disk_err:
    .ascii "Disk read error!"
    .byte 10, 13, 0

##
# Prints a zero-terminated string to the screen at the current cursor position.
#
#   Inputs: bx - address of string to print
#  Outputs: (none)
# Clobbers: ax
##
print:
    movb    $0x0e, %ah      # 0x0e - TTY mode

_print_loop:
    movb    (%bx), %al
    cmpb    $0, %al
    je      _print_done
    int     $0x10
    incw    %bx
    jmp     _print_loop

_print_done:
    ret

##
# Spins indefinitely.
##
halt:
    hlt
    jmp     halt


.section .stage2, "ax", @progbits
stage2:
    leaw    s_hello, %bx
    call    print
    jmp     halt

s_hello:
    .ascii "Hello, stage 2 world!"
    .byte 13, 10, 0
