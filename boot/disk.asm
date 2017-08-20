;===============================================================================
;        File: disk.asm
;      Author: Wes Hampson
; Description: Handles disk I/O operations during bootstrapping.
;===============================================================================

;--
; Loads the kernel image from a disk.
;
; dh    - num sectors to load
; dl    - disk id
; es:bx - data destination
;--
disk_load_kernel:
    pusha
    push        dx                  ; back up input params

    ; TODO: attempt read 3 times before declaring failure
    ;       floppy reads may not work on first attempt
    ;       reset disk before each attempt (mov ah, 0; int 0x13)
    mov         ah, 0x02            ; int 0x13 function (read disk)
    mov         al, dh              ; num sectors (0x01 to 0x80)
    mov         cl, 0x02            ; sector (0x01 to 0x11); 0x01 is boot sector
    mov         ch, 0x00            ; cylinder number (0x00 to 0x3ff)
    mov         dh, 0x00            ; head number (0x00 to 0x0f)
    int         0x13                ; do the read!
    jc          _disk_error         ; carry bit set if error

    pop         dx                  ; restore disk id and desired num sectors
    cmp         al, dh              ; compare num sectors read w/ desired num
    jne         _sectors_error      ;     ('al' set by BIOS to num sectors read)

    mov         bx, MSG_KERNEL_LOADED
    call        println
    popa
    ret

_disk_error:
    mov         bx, MSG_DISK_ERROR  ; print disk read error message
    call        println
    mov         bx, MSG_DISK
    shl         dx, 8               ; move disk id into 'dh' for hex print
    call        print
    call        println_hexb        ; print disk id
    mov         bx, MSG_CODE
    mov         dh, ah              ; error code stored in ah
    call        print
    call        println_hexb        ; print error code
    jmp         boot_fail

_sectors_error:
    mov         bx, MSG_SECTORS_ERROR
    call        println
    jmp         boot_fail

MSG_DISK_ERROR:
    db          "Disk read error.", 0

MSG_DISK:
    db          "::  disk = ", 0

MSG_CODE:
    db          "::  code = ", 0

MSG_SECTORS_ERROR:
    db          "Incorrect number of sectors read.", 0

MSG_KERNEL_LOADED:
    db          "Kernel loaded!", 0
