BUILD_PREFIX=out
GCC=i386-elf-gcc
LD=i386-elf-ld
GCCFLAGS=-g -ffreestanding -m32 -masm=intel

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:%.c=$(BUILD_PREFIX)/%.o)

qemu: $(BUILD_PREFIX)/boot.bin
	qemu-system-x86_64 $<

$(BUILD_PREFIX):
	mkdir -p out

$(BUILD_PREFIX)/%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(BUILD_PREFIX)/stage2.bin: $(BUILD_PREFIX)/stage2.o $(OBJFILES) | $(BUILD_PREFIX)/stage1a.bin
	$(LD) --oformat binary -Ttext $(shell sh -c "bc <<< \"obase=16; 32511 + $$(du -b out/stage1a.bin | cut -f1)\"") -o $@ $< $(subst $<,,$+)

$(BUILD_PREFIX)/%.bin: %.asm
	nasm -f bin $< -o $@

$(BUILD_PREFIX)/stage1.bin: stage1.asm utils.asm $(BUILD_PREFIX)/stage2.bin $(BUILD_PREFIX)/stage1a.bin
	sed $< -e "s/\(STAGE2_SIZE equ\) .*$$/\1 $(shell sh -c "bc <<< \"($$(du -b out/stage2.bin | cut -f1) + $$(du -b out/stage1a.bin | cut -f1)) / 512 + 1\"")/" > $(BUILD_PREFIX)/$<
	nasm -f bin $(@:%.bin=%.asm) -o $@

$(BUILD_PREFIX)/boot.bin: $(BUILD_PREFIX)/stage1.bin $(BUILD_PREFIX)/stage1a.bin $(BUILD_PREFIX)/stage2.bin $(BUILD_PREFIX)/nullsec.bin
	cat $^ > $@

$(BUILD_PREFIX)/nullsec.bin:
	dd if=/dev/zero bs=512B count=1 of=./$@

$(BUILD_PREFIX)/part.img:
	dd if=/dev/zero of=$@ count=16 bs=1M
	mkfs.ext2 $@

$(BUILD_PREFIX)/disk.img: $(BUILD_PREFIX)/boot.bin $(BUILD_PREFIX)/part.img
	./patch_mbr.sh $^
	cat $<.mbr $(BUILD_PREFIX)/part.img > $@

clear:
	rm $(BUILD_PREFIX)/*
