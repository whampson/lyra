;--
; Prints a zero-terminated string to the screen
; at the current cursor position.
;
; bx    - pointer to string
;--
print:
    pusha
    mov         ah, 0x0e        ; int 0x10 function (TTY output mode)

_print_loop:
    mov         al, [bx]        ; copy current character to 'al'
    cmp         al, 0           ; check for end of string...
    je          _print_done     ; ...and jump to end if true
    int         0x10            ; print the character!
    add         bx, 1           ; increment string pointer
    jmp         _print_loop

_print_done:
    popa
    ret

;--
; Prints a 16-bit number in hexadecimal to the screen
; at the current cursor position.
;
; dx    - number to print
;--
print_hex:
    pusha
    mov         cl, 4               ; loop counter
    mov         bx, HEX_STRING + 2  ; hex string pointer, skip '0x'

_print_hex_loop:
    sub         cl, 1               ; decrement counter
    js          _print_hex_done
    rol         dx, 4               ; rotate to move next digit into low 4 bits
    mov         al, dl              ; get low 8 bits of dx...
    and         al, 0x0f            ; ...and strip off upper 4 bits
    add         al, '0'             ; move number into ASCII 0-9 range
    cmp         al, '9'             ; check for A-F digit
    jle          _print_hex_store_digit
    add         al, 'A' - '0' - 10  ; move number into ASCII A-F range

_print_hex_store_digit:
    mov         [bx], al            ; store digit in string
    add         bx, 1               ; advance a character
    jmp         _print_hex_loop

_print_hex_done:
    mov         bx, HEX_STRING
    call        print               ; print it!
    popa
    ret

HEX_STRING:
    db "0x0000", 0
