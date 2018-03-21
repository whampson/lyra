#!/bin/bash

gdb boot.elf                            \
    -ex 'target remote 10.0.2.2:1234'   \
    -ex 'set architecture i8086'        \
    -ex 'break entry'                   \
    -ex 'continue'                      \
