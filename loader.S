#include "boot.h"


.section .text
kmain:
    # Put a green 'a' on the screen to prove it worked!
    movw    $0x0261, 0xB8800
    sti

pm_halt:
    hlt
    jmp     pm_halt

.globl intr_handler
intr_handler:
    incb    0xB8900
    iret
