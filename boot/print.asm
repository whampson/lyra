;===============================================================================
;        File: print.asm
;      Author: Wes Hampson
; Description: Routines for printing text to the screen during bootstrapping.
;===============================================================================

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
; Prints a carriage return, followed by a line feed
; to the screen. This causes the cursor to move to
; the start of the next line.
;--
print_newline:
    push        bx
    mov         bx, NL_STRING
    call        print
    pop         bx
    ret

;--
; Prints a zero-terminated string to the screen
; at the current cursor position, then advances the
; cursor to the start of the next line.
;
; bx    - pointer to string
;--
println:
    call        print
    call        print_newline
    ret

;--
; Prints a 16-bit number (word) to the screen in
; hexadecimal at the current cursor position.
;
; dx    - number to print
;--
print_hexw:
    pusha
    mov         cl, 4               ; loop counter
    mov         bx, HEX_STRING + 2  ; hex string pointer, skip '0x'
    jmp         _print_hex_loop

;--
; Prints an 8-bit number (byte) to the screen in
; hexadecimal at the current cursor position.
;
; dh    - number to print
;--
print_hexb:
    pusha
    mov         cl, 2               ; loop counter
    mov         bx, HEX_STRING + 2  ; hex string pointer, skip '0x'

_print_hex_loop:
    sub         cl, 1               ; decrement counter
    js          _print_hex_done
    rol         dx, 4               ; rotate to move next digit into low 4 bits
    mov         al, dl              ; get low 8 bits of dx...
    and         al, 0x0f            ; ...and strip off upper 4 bits
    add         al, '0'             ; move number into ASCII 0-9 range
    cmp         al, '9'             ; check for A-F digit
    jle         _print_hex_store_digit
    add         al, 'A' - '0' - 10  ; move number into ASCII A-F range

_print_hex_store_digit:
    mov         [bx], al            ; store digit in string
    add         bx, 1               ; advance a character
    jmp         _print_hex_loop

_print_hex_done:
    mov byte    [bx], 0
    mov         bx, HEX_STRING
    call        print               ; print it!
    popa
    ret

;--
; Prints a 16-bit number (word) to the screen in
; hexadecimal at the current cursor position, then
; advances the cursor to the start of the next line.
;
; dx    - number to print
;--
println_hexw:
    call        print_hexw
    call        print_newline
    ret

;--
; Prints an 8-bit number (byte) to the screen in
; hexadecimal at the current cursor position, then
; advances the cursor to the start of the next line.
;
; dh    - number to print
;--
println_hexb:
    call        print_hexb
    call        print_newline
    ret

; Data section
HEX_STRING:
    db          "0x", 0, 0, 0, 0, 0

NL_STRING:
    db          13, 10, 0
