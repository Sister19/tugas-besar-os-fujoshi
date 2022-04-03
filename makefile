# Makefile
all: bootloader kernel stdlib diskimage

# Recipes
diskimage:
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1

bootloader:
	nasm src/asm/bootloader.asm -o out/bootloader

kernel:
	bcc -ansi -c -o out/kernel.o src/c/kernel.c
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/std_lib.o


stdlib:
	bcc -ansi -O3 -c -o out/std_lib.o src/c/std_lib.c

run:
	bochs -f src/config/if2230.config

build-run: all run
