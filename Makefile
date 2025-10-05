BUILD_PREFIX=out

qemu: $(BUILD_PREFIX)/boot.bin
	qemu-system-x86_64 $<

$(BUILD_PREFIX):
	mkdir -p out

$(BUILD_PREFIX)/boot.bin: boot.asm utils.asm $(BUILD_PREFIX)
	nasm -f bin $< -o $@
