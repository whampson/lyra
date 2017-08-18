[ORG 0x7C00]

start:
    jmp $

; padding and magic number
times 510 - ($ - $$) db 0
dw 0xAA55
