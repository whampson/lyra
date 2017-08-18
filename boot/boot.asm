    bits        16
    org         0x7c00  ; entry point from BIOS

    %assign     STACK_BASE      0x9000
    %assign     BOOTSECT_MAGIC  0xaa55

start:
    ; store boot drive id
    mov         [BOOT_DRIVE], dl

    ; set up the stack
    mov         bp, 0x9000
    mov         sp, bp

    ; do some funky stuff
    mov         bx, MSG_HELLO
    call        print
    jmp         $               ; halt

    %include    "disk.asm"
    %include    "print.asm"

; Data section
MSG_HELLO:
    db          "Hello, world!", 13, 10, 0

BOOT_DRIVE:
    ; placeholder
    db          0

PADDING_MAGIC:
    times       510 - ($ - $$) db 0
    dw          BOOTSECT_MAGIC
