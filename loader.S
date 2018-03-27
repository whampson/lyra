#include "boot.h"

.code32
.section .text

.globl pm_entry
pm_entry:
    movw    $0x18, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs

    movl    $0x7C00, %ebp
    movl    %ebp, %esp

    lidtl   idt_desc

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
    sti

pm_halt:
    hlt
    jmp     pm_halt

.globl intr_handler
intr_handler:
    incb    0xB8900
    iret
