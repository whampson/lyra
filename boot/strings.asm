;===============================================================================
;        File: disk.asm
;      Author: Wes Hampson
; Description: Messages shown during boot.
;===============================================================================

MSG_DISK_ERROR:
    db          "Disk read error.", 0

MSG_DISK_ERROR_DISK:
    db          "::  disk = ", 0

MSG_DISK_ERROR_CODE:
    db          "::  code = ", 0

MSG_SECTORS_ERROR:
    db          "Incorrect sector count of sectors read.", 0

MSG_BOOT_FAIL:
    db          "BOOT FAILED", 0
