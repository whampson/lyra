;===============================================================================
;        File: boot.asm
;      Author: Wes Hampson
; Description: OS bootstrap sequence.
;===============================================================================

    bits        16
    org         0x7c00              ; entry point after BIOS execution

    %assign     STACK_BASE      0x9000
    %assign     STACK_BASE_PM   0x90000
    %assign     KERNEL_BASE     0x1000
    %assign     BOOTSECT_MAGIC  0xaa55

start_boot:
    ; store boot drive id
    mov         [BOOT_DRIVE], dl

    ; set up the stack
    mov         bp, STACK_BASE
    mov         sp, bp

    ; load the kernel from disk
    mov         bx, KERNEL_BASE     ; data destination
    mov         dl, [BOOT_DRIVE]    ; disk id
    mov         dh, 2               ; num sectors to read
    call        disk_load_kernel

    ; switch into protected mode
    cli                             ; clear interrupts... no more BIOS calls!
    lgdt        [THE_GDT]           ; load the GDT
    mov         eax, cr0
    or          eax, 1              ; set 32-bit mode ('protected' mode)
    mov         cr0, eax
    jmp         CODE_SEGMENT:kernel_start

boot_fail:
    mov         bx, MSG_BOOT_FAIL
    call        print_newline
    call        println
    jmp         $

    %include    "disk.asm"
    %include    "gdt.asm"
    %include    "print.asm"

;===============================================================================

    bits        32

kernel_start:
    ; update segment registers
    mov         ax, DATA_SEGMENT
    mov         ds, ax
    mov         ss, ax
    mov         es, ax
    mov         fs, ax
    mov         gs, ax

    ; put stack base at the top of free space
    mov         ebp, STACK_BASE_PM
    mov         esp, ebp

    jmp         $

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