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
SRCEXT_C        := c
OBJEXT          := o

# Target executables
TARGET_BOOT     := boot.bin
TARGET_KERNEL   := kernel.bin
TARGET_IMAGE    := betelgeuse.img

################################################################################

SOURCES_BOOT    := boot/boot.$(SRCEXT_ASM)

SOURCES_KERNEL  := $(SRCDIR)/boot/kernel_loader.$(SRCEXT_ASM)
SOURCES_KERNEL	+= $(shell find $(SRCDIR)/kernel -type f -name "*.$(SRCEXT_C)")
OBJECTS_KERNEL  := $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SOURCES_KERNEL))
OBJECTS_KERNEL  := $(OBJECTS_KERNEL:.$(SRCEXT_C)=.$(OBJEXT))
OBJECTS_KERNEL  := $(OBJECTS_KERNEL:.$(SRCEXT_ASM)=.$(OBJEXT))

KERNEL_BASE     := 0x1000

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
	$(eval OBJ = $(addprefix $(BINDIR)/, $^))
	cat $(OBJ) > $(BINDIR)/$@

$(TARGET_BOOT): $(SOURCES_BOOT)
	$(AS) $(ASFLAGS) -i boot/ -f bin -o $(BINDIR)/$@ $^

$(TARGET_KERNEL): $(OBJECTS_KERNEL)
	$(LD) $(LDFLAGS) -Ttext $(KERNEL_BASE) --oformat binary -o $(BINDIR)/$@ $^

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT_ASM)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -f elf -o $@ $^

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT_C)
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -ffreestanding -c -o $@ $^

.PHONY: all directories clean tidy run
