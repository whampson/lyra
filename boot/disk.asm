;--
; Loads the kernel image from a disk
;
; dh    - disk id
; dl    - num sectors to load
; es:bx - data destination
;--
disk_load_kernel:
    pusha
    push        dx                  ; back up input params
    
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

    popa
    ret

_disk_error:
_sectors_error:
    ; TODO
    jmp $