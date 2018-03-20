.code16
.section .stage1, "ax", @progbits

STACK_BASE  = 0x7C00
STAGE2_SEG  = 0x07E0
STAGE2_OFF  = 0x0000
SECT_COUNT  = 1
RETRY_COUNT = 3

# TODO: (bootloader)
#       enable prot. mode
#           enable A20
#           setup GDT
#           disable interrupts (NMI)
#           write cr0
#           reset segment registers
#       load kernel
#           floppy disk driver
#       jump to kernel entry point
# TODO: (kernel)
#       load IDT and other x86 structures
#       enable paging

##
# Bootloader entry point -- where it all begins!
##
.globl entry
entry:
    cli
    movw    $STACK_BASE, %bp
    movw    %bp, %sp

    call    load_stage2
    call    a20_enable
    jmp     stage2

a20_enable:
    # Actually, let's disable it first (as a test).
    # QEMU (or rather, SeaBIOS) seems to enable A20 by default.
    call    a20_disable

    call    a20_test_enabled
    cmpw    $0, %ax
    je      _a20_enable_i8042
    leaw    s_a20_enabled, %bx
    jmp     _dbg_a20_enable_done

    # TODO: enable A20 (keyboard)
_a20_enable_jmptbl:
    .word   _a20_enable_i8042

_a20_enable_i8042:
    pushw   %ax
    call    i8042_out_p
    movb    $0xD0, %al      # cmd: read PS/2 output port
    outb    %al, $0x64      # send cmd
    call    i8042_in_p
    inb     $0x60, %al      # data in: output port status
    movb    %al, %ah
    call    i8042_out_p
    movb    $0xD1, %al      # cmd: write PS/2 output port
    outb    %al, $0x64      # send cmd
    call    i8042_out_p
    orb     $0x02, %ah      # set A20 bit
    movb    %ah, %al        # data out: old output port status w/ A20 cleared
    outb    %al, $0x60      # send data
    popw    %ax
    ret

_dbg_a20_disabled:
    leaw    s_a20_disabled, %bx

_dbg_a20_enable_done:
    call    print
    ret

s_a20_disabled:
    .ascii "A20 line disabled!"
    .byte 13, 10, 0

s_a20_enabled:
    .ascii "A20 line enabled!"
    .byte 13, 10, 0

##
# Disables the A20 address line (PS/2 keyboard method).
#
#   Inputs: (none)
#  Outputs: (none)
# Clobbers: (none)
##
a20_disable:
    pushw   %ax
    call    i8042_out_p
    movb    $0xD0, %al      # cmd: read PS/2 output port
    outb    %al, $0x64      # send cmd
    call    i8042_in_p
    inb     $0x60, %al      # data in: output port status

    # Check if A20 is already disabled
    test    $0x02, %al
    jz      _a20_disable_done
    movb    %al, %ah

    call    i8042_out_p
    movb    $0xD1, %al      # cmd: write PS/2 output port
    outb    %al, $0x64      # send cmd
    call    i8042_out_p
    andb    $0xFD, %ah      # clear A20 bit
    movb    %ah, %al        # data out: old output port status w/ A20 cleared
    outb    %al, $0x60      # send data

_a20_disable_done:
    popw    %ax
    ret

##
# Tests whether the A20 address line is enabled.
#
# Methodology:
#   Test whether memory "wraps around" when using a 21-bit segmented
#   address. We can use the bootsector identifier as a marker. If A20 is NOT
#   enabled, segment addresses 0000:7CFE and FFFF:7EOE will map to the same
#   physical address (whose value is 0xAA55, i.e. the bootsector ID). If
#   the values at these two addresses are different, then the A20 line is
#   enabled.
#
#   Inputs: (none)
#  Outputs: ax set to 1 if enabled,
#           ax set to 0 if disabled
# Clobbers: ax, es
##
a20_test_enabled:
    pushw   %bx
    pushw   %cx
    movw    $1, %cx

_a20_test_loop:
    movw    $0xFFFF, %ax
    movw    %ax, %es
    movw    %es:0x7E0E, %bx         # read "wrapped around" address
    xorw    %ax, %ax
    movw    %ax, %es
    cmpw    %bx, %es:0x7DFE         # compare with bootsect ID
    jne     _a20_test_true

    # The values were equal! Ensure it wasn't a fluke.
    # Alter the bootsector ID and recompare.
    cmpw    $0, %cx
    je      _a20_test_done
    decw    %cx
    rorw    $8, %bx
    movw    %bx, %es:0x7DFE
    jmp     _a20_test_loop

_a20_test_true:
    movw    $1, %ax

_a20_test_done:
    popw    %cx
    popw    %bx
    ret

##
# Loads the second stage of the bootloader from the boot floppy.
# If the load fails, an error message will be displayed and execution will
# cease.
#
#   Inputs: (none)
#  Outputs: Stage 2 boot code starting at STAGE2_SEG:STAGE2_OFF.
# Clobbers: ax, bx, cx, dx, es, si
##
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
# Clobbers: bx
##
print:
    pushw   %ax
    movb    $0x0e, %ah          # 0x0e - TTY mode

_print_loop:
    movb    (%bx), %al
    cmpb    $0, %al
    je      _print_done
    int     $0x10
    incw    %bx
    jmp     _print_loop

_print_done:
    popw    %ax
    ret

##
# Spin indefinitely.
##
halt:
    hlt
    jmp     halt


.section .stage2, "ax", @progbits
stage2:
    # PLACEHOLDER
    leaw    s_hello, %bx
    call    print
    jmp     halt

s_hello:
    .ascii "Hello, stage 2 world!"
    .byte 13, 10, 0


#### Misc. stuff

##
# Delay execution for a jiffy.
##
delay:
    pushw   %cx
    movw    $0xFFFF, %cx
_delay_loop:
    decw    %cx
    jnz     _delay_loop
    popw    %cx
    ret

##
# Wait for PS/2 controller to be ready before reading from data port.
##
i8042_in_p:
    pushw   %ax
_i8042_in_p_loop:
    call    delay
    inb     $0x64, %al
    test    $0x01, %al          # check output buffer status (1 = ready)
    jz      _i8042_in_p_loop
    popw    %ax
    ret

##
# Wait for PS/2 controller to be ready before writing to command or data port.
##
i8042_out_p:
    pushw   %ax
_i8042_out_p_loop:
    call    delay
    inb     $0x64, %al
    test    $0x02, %al          # check input buffer status (0 = ready)
    jnz     _i8042_out_p_loop
    popw    %ax
    ret
