.PHONY: all img boot kernel

all: img

img: boot kernel
	cat boot/boot.bin kernel/kernel.bin > lyra.img

boot:
	$(MAKE) -C boot/

kernel:
	(cd kernel/; sh -c ./build.sh)

clean:
	$(MAKE) -C boot/ clean
	(cd kernel/; rm -f *.o *.elf *.bin)
	rm -f lyra.img

install: img
	dd if=lyra.img of=/dev/fd0
