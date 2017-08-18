# Toolchain
AS              := nasm
EMU             := qemu-system-i386
LD              := cat  # yes, you read that correctly!
                        # we're just joining binary files
# Flags
ASFLAGS         :=
EMUFLAGS        := -boot a

# Directories
SRCDIR          := .
OBJDIR          := obj
BINDIR          := bin

# Extensions
SRCEXT_ASM      := asm
OBJEXT          := o

# Target executables
TARGET_BOOT     := boot.bin
TARGET_IMAGE    := betelgeuse.img

################################################################################

SOURCES_BOOT    := boot.asm
SOURCES_BOOT    := $(patsubst %, $(SRCDIR)/boot/%, $(SOURCES_BOOT))

all: directories $(TARGET_BOOT)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

clean:
	@rm -rf $(OBJDIR)

tidy: clean
	@rm -rf $(BINDIR)

run: all $(TARGET_IMAGE)
	$(EMU) $(EMUFLAGS) -fda $(BINDIR)/$(TARGET_IMAGE)

$(TARGET_BOOT): $(SOURCES_BOOT)
	$(AS) $(ASFLAGS) -i boot/ -f bin -o $(BINDIR)/$@ $^

$(TARGET_IMAGE): $(TARGET_BOOT)
	$(LD) $(BINDIR)/$^ > $(BINDIR)/$@

.PHONY: all directories clean tidy run
