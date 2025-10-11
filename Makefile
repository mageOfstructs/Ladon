BUILD_PREFIX=out
GCC=i386-elf-gcc
LD=i386-elf-ld
GCCFLAGS=-g -ffreestanding -m32 -masm=intel
QEMUFLAGS=-chardev file,id=blog,path=./boot.log -serial chardev:blog

HDRFILES=$(wildcard *.h fs/*.h)
SRCFILES=$(wildcard *.c fs/*.c)
OBJFILES=$(SRCFILES:%.c=$(BUILD_PREFIX)/%.o)

qemu: $(BUILD_PREFIX)/disk.img
	qemu-system-x86_64 $(QEMUFLAGS) $<

$(BUILD_PREFIX):
	mkdir -p out/fs

$(BUILD_PREFIX)/%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(BUILD_PREFIX)/stage2.bin: $(BUILD_PREFIX)/stage2.o $(OBJFILES) | $(BUILD_PREFIX)/stage1a.bin
	#																																32511 = 0x7eFF
	$(LD) --oformat binary -Ttext $(shell sh -c "bc <<< \"obase=16; 32511 + $$(du -b out/stage1a.bin | cut -f1)\"") -o $@ $< $(subst $<,,$+)

$(BUILD_PREFIX)/%.bin: %.asm
	nasm -f bin $< -o $@

$(BUILD_PREFIX)/stage1.bin: stage1.asm utils.asm $(BUILD_PREFIX)/stage2.bin $(BUILD_PREFIX)/stage1a.bin
	sed $< -e "s/\(STAGE2_SIZE equ\) .*$$/\1 $(shell sh -c "bc <<< \"($$(du -b out/stage2.bin | cut -f1) + $$(du -b out/stage1a.bin | cut -f1)) / 512 + 1\"")/" > $(BUILD_PREFIX)/$<
	nasm -f bin $(@:%.bin=%.asm) -o $@

$(BUILD_PREFIX)/preboot.bin: $(BUILD_PREFIX)/stage1.bin $(BUILD_PREFIX)/stage1a.bin $(BUILD_PREFIX)/stage2.bin
	cat $^ > $@

$(BUILD_PREFIX)/boot.bin: $(BUILD_PREFIX)/preboot.bin $(BUILD_PREFIX)/nullsec.bin
	cat $^ > $@

$(BUILD_PREFIX)/nullsec.bin: $(BUILD_PREFIX)/preboot.bin
	dd if=/dev/zero bs=$(shell sh -c "bc <<< \"(1024 - $$(du -b $< | cut -f1) % 1024) % 1024\"")B count=1 of=./$@

$(BUILD_PREFIX)/test_elf: test/main.c
	$(GCC) $(GCCFLAGS) -c -o $(subst .c,.o,$<) $<
	$(LD) $(subst .c,.o,$<) -o $@

$(BUILD_PREFIX)/part.img: $(BUILD_PREFIX)/test_elf
	dd if=/dev/zero of=$@ count=16 bs=1M
	mkfs.ext2 $@
	sudo mkdir -p /mnt/tmp
	sudo mount $@ /mnt/tmp
	sudo sh -c "echo \"Hello World!\" >> /mnt/tmp/hello"
	sudo cp $< /mnt/tmp
	sudo umount /mnt/tmp

$(BUILD_PREFIX)/disk.img: $(BUILD_PREFIX)/boot.bin $(BUILD_PREFIX)/part.img
	./patch_mbr.sh $^
	cat $<.mbr $(BUILD_PREFIX)/part.img > $@

clear: $(BUILD_PREFIX)
	rm -f $(BUILD_PREFIX)/*.*
