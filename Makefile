.PHONY: all img boot

all: img

img: boot
	cat boot/boot.bin kernel/kernel.bin > lyra.img

boot:
	$(MAKE) -C boot/

clean:
	$(MAKE) -C boot/ clean
	@rm lyra.img
