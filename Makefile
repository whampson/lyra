#-------------------------------------------------------------------------------
# Copyright (C) 2018 Wes Hampson
#
# File: Makefile
# Desc: Bootloader build script.
#-------------------------------------------------------------------------------

AS          := gcc
ASFLAGS     := -m32 -ffreestanding
LD          := ld
LDSCRIPT    := bootloader.ld
LDFLAGS     := -T $(LDSCRIPT)

ASM_SOURCES := $(wildcard *.S)
OBJECTS     := $(ASM_SOURCES:.S=.o)

ELFTARGET   := boot.elf
BINTARGET   := boot.bin

.PHONY: all debug clean remake

all: $(BINTARGET)

$(BINTARGET): $(ELFTARGET)
	objcopy -O binary $< $@

$(ELFTARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

debug: ASFLAGS += -g
debug: remake

clean:
	@rm -f *.o *.elf *.bin

remake: clean all
