#-------------------------------------------------------------------------------
# Copyright (C) 2018 Wes Hampson
#
# File: pm.S
# Desc: <TODO>
#-------------------------------------------------------------------------------

.code16
.section .stage2, "ax", @progbits

.align 16
gdt_start:
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # 0x08 -- KERNEL_CS
    .quad   0x00CF92000000FFFF      # 0x10 -- KERNEL_DS
    .quad   0x00CFFA000000FFFF      # 0x18 -- USER_CS
    .quad   0x00CFF2000000FFFF      # 0x20 -- USER_DS
    .quad   0x0000000000000000
    .quad   0x0000000000000000
gdt_end:

gdt_desc:
    .word   gdt_end - gdt_start - 1
    .long   gdt_start

.globl pm_switch
pm_switch:
    call    a20_enable
    lgdt    gdt_desc

    # Here we go...!
    movl    %cr0, %eax
    orw     $0x01, %ax
    movl    %eax, %cr0
    ljmp    $0x08, $pm_entry

.code32
pm_entry:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss

    # Put a green 'a' on the screen to prove it worked!
    movw    $0x0261, 0xB8800

pm_halt:
    hlt
    jmp     pm_halt
