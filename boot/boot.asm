[ORG 0x7c00]

start:
    mov bx, MSG_HELLO
    call print
    jmp $

%include "print.asm"

MSG_HELLO db "Hello, world!", 13, 10, 0

; padding and magic number
times 510 - ($ - $$) db 0
dw 0xaa55
