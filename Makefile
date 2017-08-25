# Toolchain
AS              := nasm
CC              := i386-elf-gcc
EMU             := qemu-system-i386
LD              := i386-elf-ld

# Flags
ASFLAGS         :=
CCFLAGS         :=
EMUFLAGS        := -boot a
LDFLAGS         :=

# Directories
SRCDIR          := .
OBJDIR          := obj
BINDIR          := bin

# Extensions
SRCEXT_ASM      := asm
SRCEXT_C		:= c
OBJEXT          := o

# Target executables
TARGET_BOOT     := boot.bin
TARGET_KERNEL   := kernel.bin
TARGET_IMAGE    := betelgeuse.img

################################################################################

SOURCES_BOOT    := boot/boot.$(SRCEXT_ASM)

SOURCES_KERN_ASM := $(shell find $(SRCDIR)/kernel -type f -name "*.$(SRCEXT_ASM)")
OBJECTS_KERN_ASM := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SOURCES_KERN_ASM:.$(SRCEXT_ASM)=.$(OBJEXT)))

SOURCES_KERN_C  := $(shell find $(SRCDIR)/kernel -type f -name "*.$(SRCEXT_C)")
OBJECTS_KERN_C  := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SOURCES_KERN_C:.$(SRCEXT_C)=.$(OBJEXT)))

all: directories $(TARGET_IMAGE)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

clean:
	@rm -rf $(OBJDIR)

tidy: clean
	@rm -rf $(BINDIR)

run: all
	$(EMU) $(EMUFLAGS) -fda $(BINDIR)/$(TARGET_IMAGE)

$(TARGET_IMAGE): $(TARGET_BOOT) $(TARGET_KERNEL)
	cat $(BINDIR)/$(TARGET_BOOT) $(BINDIR)/$(TARGET_KERNEL) > $(BINDIR)/$@

$(TARGET_BOOT): $(SOURCES_BOOT)
	$(AS) $(ASFLAGS) -i boot/ -f bin -o $(BINDIR)/$@ $^

$(TARGET_KERNEL): $(OBJECTS_KERN_ASM) $(OBJECTS_KERN_C)
	$(LD) $(LDFLAGS) -Ttext 0x1000 --oformat binary -o $(BINDIR)/$@ $^

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT_ASM)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -f elf -o $@ $^

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT_C)
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -ffreestanding -c -o $@ $^

.PHONY: all directories clean tidy run
