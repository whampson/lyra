    section     .text
    global      _start
    extern      kernel_main
    bits        32

_start:
    call        kernel_main
    jmp         $
