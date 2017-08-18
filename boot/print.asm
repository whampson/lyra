%ifndef __PRINT_ASM__
%define __PRINT_ASM__

;--
; Prints a zero-terminated string to the screen
; at the current cursor position
;
; bx    - pointer to string
;--
print:
    pusha
    mov ah, 0x0e        ; int 0x10 function (TTY output mode)

_print_loop:
    mov al, [bx]        ; copy current character to 'al'
    cmp al, 0           ; check for end of string...
    je _print_done      ; ...and jump to end if true
    int 0x10            ; print the character!
    add bx, 1           ; increment string pointer
    jmp _print_loop

_print_done:
    popa
    ret

%endif