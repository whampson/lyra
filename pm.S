#-------------------------------------------------------------------------------
# Copyright (C) 2018 Wes Hampson
#
# File: pm.S
# Desc: <TODO>
#-------------------------------------------------------------------------------

.code16
.section .stage2, "ax", @progbits

.globl pm_switch
pm_switch:
    call    a20_enable
    lgdtw   the_gdt
    # TODO: the rest...

foo:
    leaw    s_hello, %bx
    call    print
    jmp     halt

s_hello:
    .ascii  "Hello, stage 2 world!"
    .byte   13, 10, 0

.align 16
the_gdt:
    .quad   0x0000000000000000
    .quad   0x00CF9A000000FFFF      # 0x08 -- KERNEL_CS
    .quad   0x00CF92000000FFFF      # 0x10 -- KERNEL_DS
    .quad   0x00CFFA000000FFFF      # 0x18 -- USER_CS
    .quad   0x00CFF2000000FFFF      # 0x20 -- USER_DS
    .quad   0x0000000000000000
    .quad   0x0000000000000000