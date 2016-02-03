#!/bin/bash

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

BADGE="${GRN} INFO ${BLK}"
WARN="${RED} WARNING ${BLK}"
VERSION="Builder v2.1"


function build {
	printf "[${BADGE}] ${VERSION}\n"
	if assemble && compile && linker && booter; then
		printf "[${BADGE}] Running kernel emulator...\n"
		qemu-system-i386 -fda popcorn.img --no-kvm
		printf "[${BADGE}] Finished.\n"
	else
		printf "[${BADGE}]${RED} Build failed! ${BLK}\n"
	fi
}

function assemble {
	printf "[${BADGE}] Assembling...\n"
	nasm -f elf src/boot.asm -o obj/boot.o
}

function compile {
	printf "[${BADGE}] Compiling...\n"
	#g++ -m32 -c src/main.c -o obj/kernel.o -ffreestanding -Wall -Wextra -Werror -O1
	g++ -m32 -c src/main.c -o obj/kernel.o -ffreestanding -Wfatal-errors
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

function tousb {
		sudo mount /dev/sda -o loop /media/USB/
		if sudo cp bin/kernel /media/USB/boot/kernel/;
		then printf "[${BADGE}] Writing bootable image to USB drive /dev/sda...\n"
		else printf "[${INFO}] Failed to write!\n"
		fi
		sudo mv /media/USB/boot/kernel/kernel /media/USB/boot/kernel/popcorn-1.2.bin
}

# Call the build process
build
 