#include "boot.h"

.code32
.section .text

.globl pm_entry
pm_entry:
    movl    $0x7C00, %esp
    movw    $0x18, %ax
    movw    %ax, %ss
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    jmp     kmain

move_kernel:
    movl    $KERNEL_NUM_SECTORS, %ecx
    shll    $9, %ecx
    movl    $KERNEL_TEMP_BASE, %esi
    movl    $0x00400000, %edi
rep movsb   (%esi), (%edi)

    jmp     0x00400000


kmain:
    # Put a green 'a' on the screen to prove it worked!
    movw    $0x0261, 0xB8800

pm_halt:
    jmp     pm_halt
