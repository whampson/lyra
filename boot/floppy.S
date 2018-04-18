#include "floppy.h"

.section .stage1, "ax", @progbits
.code16

##
# Loads sectors of contiguous data from the boot disk to the address specified
# by 0000:bx.
# Each sector is 512 bytes.
#
# LIMITATIONS:
#   As of now, this routine only loads sectors from the first track of the
#   first head of the floppy. Therefore, only 18 sectors can be loaded at a
#   time.
#
#   Inputs: bx - destination address
#           cl - starting sector
#           ch - number of sectors
#  Outputs: ah - 0 for success, nonzero if an error occurred
# Clobbers: ax, ch, dx, es, si
##
read_sectors:
    xorw    %ax, %ax
    movw    %ax, %es
    movw    $FLOPPY_NUM_RETRIES, %si
    movb    %ch, %al

_read_sectors_loop:
    decw    %si
    movb    $0x02, %ah                  # func: read floppy
    movb    $0, %ch                     # cylinder number
    movb    $0, %dh                     # head number
    movb    $0, %dl                     # drive number
    int     $0x13
    jc      _read_sectors_retry
    cmpb    $0, %ah
    jnz     _read_sectors_retry
    ret

_read_sectors_retry:
    cmpw    $0, %si
    je      _read_sectors_error
    jmp     _read_sectors_loop

_read_sectors_error:
    ret

##
# Prints a disk read error message and halts.
##
disk_err:
    leaw    s_disk_err, %bx
    call    print
    jmp     halt

s_disk_err:
    .ascii  "Disk read error! Unable to boot."
    .byte   10, 13, 0
