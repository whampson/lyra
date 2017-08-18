# Toolchain
AS              := nasm
EMU             := qemu-system-i386

# Flags
ASFLAGS         :=
EMUFLAGS        :=

# Directories
SRCDIR          := .
OBJDIR          := obj
BINDIR          := bin

# Extensions
SRCEXT_ASM      := asm
OBJEXT          := o

# Target executables
TARGET_BOOT     := boot.bin

################################################################################

SOURCES_BOOT    := $(SRCDIR)/boot/boot.$(SRCEXT_ASM)

all: directories $(TARGET_BOOT)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

clean:
	@rm -rf $(OBJDIR)

tidy: clean
	@rm -rf $(BINDIR)

run: all
	$(EMU) $(EMUFLAGS) -fda $(BINDIR)/$(TARGET_BOOT)

$(TARGET_BOOT): $(SOURCES_BOOT)
	$(AS) $(ASFLAGS) -i boot/ -f bin -o $(BINDIR)/$(TARGET_BOOT) $^

.PHONY: all directories clean tidy run
