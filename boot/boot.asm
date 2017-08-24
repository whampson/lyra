;===============================================================================
;        File: boot.asm
;      Author: Wes Hampson
; Description: OS bootstrap sequence.
;===============================================================================

    bits        16
    org         0x7c00              ; entry point after BIOS execution

    %assign     STACK_BASE      0x09000
    %assign     STACK_BASE_PM   0x90000
    %assign     KERNEL_BASE     0x1000
    %assign     BOOTSECT_MAGIC  0xaa55

start_boot:
    ; store boot drive id
    mov         [BOOT_DRIVE], dl

    ; set up the stack
    mov         bp, STACK_BASE
    mov         sp, bp

    ; set up segment registers
    mov         ax, 0
    mov         es, ax

    ; load kernel and switch to 32-bit mode
    call        kernel_load
    jmp         pm_switch

kernel_load:
    ; load the kernel from disk
    mov         bx, KERNEL_BASE     ; data destination
    mov         dl, [BOOT_DRIVE]    ; disk id
    mov         dh, 16              ; num sectors to read
    call        disk_load_kernel
    ret

pm_switch:
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

kernel_start:
    bits        32
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

clear_screen:
    mov         eax, 0xb8000
    mov         ecx, 25 * 80

clear_screen_loop:
    mov    byte [eax], 0x00        ; char
    mov    byte [eax + 1], 0x07    ; attr
    add         eax, 2
    sub         ecx, 1
    jz          halt
    jmp         clear_screen_loop

halt:
    jmp         $

    %include    "disk.asm"
    %include    "gdt.asm"
    %include    "print.asm"
    %include    "strings.asm"

; Data section
BOOT_DRIVE:
    ; placeholder
    db          0

PADDING_MAGIC:
    times       510 - ($ - $$) db 0
    dw          BOOTSECT_MAGIC
