#!/bin/bash

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

BADGE="${GRN} INFO ${BLK}"
VERSION="Builder v1.4"


function build {
	printf "[${BADGE}] ${VERSION}\n"
	if assemble && compile && linker && booter; then
		printf "[${BADGE}] Running kernel emulator...\n"
		qemu-system-i386 -fda popcorn.img --no-kvm
		echo "[${BADGE}] Finished.\n"
	else
		printf "[${BADGE}]${RED} Build failed! ${BLK}\n"
	fi
}

function ok {
	printf "\t${GRN} OK ${BLK}\n"
}

function fail {
	printf "\t${RED} FAIL ${BLK}\n"
}

function assemble {
	printf "[${BADGE}] Assembling..."
	nasm -f elf src/boot.asm -o obj/boot.o
}

function compile {
	printf "[${BADGE}] Compiling...\n"
	gcc -m32 -c src/main.c -o obj/kernel.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror -O1
}

function linker {
	printf "[${BADGE}] Linking...\n"
	ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel obj/boot.o obj/kernel.o
}

function booter {
	printf "[${BADGE}] Creating bootable image...\n"
	sudo mount popcorn.img -o loop /media/floppy
	sudo cp bin/kernel /media/floppy/boot/kernel/
	sudo mv /media/floppy/boot/kernel/kernel /media/floppy/boot/kernel/popcorn-1.2.bin
	sudo umount /media/floppy
}

# Call the build process
build
 