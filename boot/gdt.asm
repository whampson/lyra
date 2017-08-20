;===============================================================================
;        File: gdt.asm
;      Author: Wes Hampson
; Description: Routines for setting up the Global Descriptor Table (GDT).
;              The main purpose of this specific GDT configuration is to make
;              all 4 GiB of memory accessible to the CPU.
;===============================================================================

; TODO: Document GDT memory layout

CODE_SEGMENT    equ gdt_code - gdt_start
DATA_SEGMENT    equ gdt_data - gdt_start

gdt_start:
    ; GDT begins with a null entry
    dd          0x00000000
    dd          0x00000000

gdt_code:
    ; add GDT entry for code section
    dw          0xffff      ; segment limit     bits 15:0
    dw          0x0000      ; segment base      bits 15:0
    db          0x00        ; segment base      bits 23:16
    db          0b10011010  ; access flags
    db          0b11001111  ; other flags
    db          0x00        ; segment base      bits 31:24

gdt_data:
    ; add GDT entry for data section
    dw          0xffff      ; segment limit     bits 15:0
    dw          0x0000      ; segment base      bits 15:0
    db          0x00        ; segment base      bits 23:16
    db          0b10010010  ; access flags
    db          0b11001111  ; other flags
    db          0x00        ; segment base      bits 31:24

gdt_end:
    ; dummy label; keep for computing offsets!!

THE_GDT:
    dw          gdt_end - gdt_start - 1
    dd          gdt_start
