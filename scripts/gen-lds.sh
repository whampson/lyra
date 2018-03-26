#!/bin/bash

if [ $# -lt 1 ]; then
    echo "$0: usage: input_ld output_ld cflags"
    exit 1
fi

gcc -E -x c $1 | grep -v '^#' > $2
