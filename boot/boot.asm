;===============================================================================
;        File: boot.asm
;      Author: Wes Hampson
; Description: OS bootstrap sequence.
;===============================================================================

    bits        16
    org         0x7c00              ; entry point after BIOS execution

    %assign     STACK_BASE      0x9000
    %assign     BOOTSECT_MAGIC  0xaa55

start:
    ; store boot drive id
    mov         [BOOT_DRIVE], dl

    ; set up the stack
    mov         bp, STACK_BASE
    mov         sp, bp

    ; load the kernel from disk
    mov         bx, 0xA000          ; data destination
    mov         dl, [BOOT_DRIVE]    ; disk id
    mov         dh, 2               ; num sectors to read
    call        disk_load_kernel
    mov         cx, 8

data_print_loop:
    mov         dx, [bx]
    call        println_hexw
    add         bx, 2
    sub         cx, 1
    jnz         data_print_loop
    jmp         halt

boot_fail:
    mov         bx, MSG_BOOT_FAIL
    call        print_newline
    call        println

halt:
    jmp         $

    %include    "disk.asm"
    %include    "print.asm"

; Data section
MSG_BOOT_FAIL:
    db          "BOOT FAILED", 0

BOOT_DRIVE:
    ; placeholder
    db          0

PADDING_MAGIC:
    times       510 - ($ - $$) db 0
    dw          BOOTSECT_MAGIC

; onto the next sector!
    dd          0xcafebabe
    dd          0x0badbeef
    dd          0xba5eba77
    dd          0x00c0ffee