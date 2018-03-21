#-------------------------------------------------------------------------------
# Copyright (C) 2018 Wes Hampson
#
# File: Makefile
# Desc: Bootloader build script.
#-------------------------------------------------------------------------------

AS 			:= gcc
ASFLAGS 	:= -m32 -ffreestanding
LD 			:= ld
LDSCRIPT	:=
LDFLAGS 	:= -T bootloader.ld

ASMEXT		:= S
SOURCES		:= $(wildcard *.$(ASMEXT))
OBJECTS		:= $(patsubst %.$(ASMEXT), %.o, $(SOURCES))

ELFTARGET	:= boot.elf
BINTARGET	:= boot.bin

.PHONY: all
all: $(BINTARGET)

$(BINTARGET): $(ELFTARGET)
	objcopy -O binary $< $@

$(ELFTARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	@rm -f *.o *.elf *.bin