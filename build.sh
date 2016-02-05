#!/bin/bash

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

BADGE="${GRN} v3.2 ${BLK}"
WARN="${RED} WARNING ${BLK}"
R="${RED}*${BLK}"
VERSION="Popcorn Builder Tool v3.2"
SWITCH=$1
TARGET=$2
FLAG=$3

# Syntax: 'build [-switch] target
#	Switches: 
#	-image		Update usb128.img
#	-reinstall	Reinstall grub to popcorn.img
#	-w		Write popcorn.img to /dev/sda
#	-e		Execute emulator
#	-s		Write to stable binary

function printhelp {
	printf "\n"
	printf "$GRN--${VERSION}--$BLK\n"
	printf "Syntax:  'build [-switch] [target] [-q/-Q]'\n"
	printf " -c  --compile     Compile to kernel [target]\n"
	printf " -i  --image       Update kernel [target] on local image\n"
	printf "$R-d  --device      Update kernel [target] on device\n"
	printf "$R-w  --write       Write local image to device\n"
	printf " -h  --help        Print this text\n"
	printf " -q  --qemu-img    Run QEMU using image\n"
	printf "$R-Q  --qemu-dev    Run QEMU using device\n"
	printf "Valid targets: 'stable', 'beta' (default)\n"
	printf "Commands with ($R) may require root password\n\n"
}

function fail {
	printf "[$RED FAIL $BLK] $1\n"
}

function main {
		if [[ "$SWITCH" = "-c" || "$SWITCH" = "--compile" ]]; then
			{ assemble && compile && link; } || { fail "Aborting" && exit; }
		fi

		if [[ "$SWITCH" = "-i" || "$SWITCH" = "--image" ]]; then
			image
		fi
		
		if [[ "$SWITCH" = "-d" || "$SWITCH" = "--device" ]]; then
			device
		fi
	
		if [[ "$SWITCH" = "-w" || "$SWITCH" = "--write" ]]; then
			writeimg
		fi
	
		if [[ "$SWITCH" = "-q" || "$SWITCH" = "--qemu-img" ]]; then
			qemuimage
		fi
		
		if [[ "$FLAG" = "-q" || "$FLAG" = "--qemu-img" ]]; then
			qemuimage
		fi
		
		if [[ "$FLAG" = "-Q" || "$FLAG" = "--qemu-dev" ]]; then
			qemudev
		fi
	
		if [[ "$SWITCH" = "-Q" || "$SWITCH" = "--qemu-dev" ]]; then
			qemudev
		fi
	
		if [[ "$SWITCH" = "-h" || "$SWITCH" = "--help" || "$SWITCH" = "" ]]; then
			printhelp
		fi
		
}

function assemble {
	printf "[${BADGE}] Assembling\n"
	nasm -f elf src/boot.asm -o obj/boot.o
}

function compile {
	printf "[${BADGE}] Compiling\n"
	#echo
	g++ -m32 -c src/main.c -o obj/kernel.o -std=c++03 -ffreestanding -Wall -Wextra
}

function link {
	if [ "$TARGET" = "stable" ] ; then
		printf "[$BADGE] Linking stable kernel\n"
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-stable obj/boot.o obj/kernel.o
	else
		printf "[$BADGE] Linking stable kernel\n"
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-beta obj/boot.o obj/kernel.o 
	fi
}

function device {
	sudo mount /dev/sda1 /media/usb128/sda
	if [ "$TARGET" = "stable" ] ; then
		printf "[$BADGE] Writing stable kernel to USB\n"
		sudo cp bin/kernel-stable /media/usb128/sda/boot/popcorn-stable
		printf "[$BADGE] Writing stable kernel to USB\n"
		sudo cp bin/kernel-beta /media/usb128/sda/boot/popcorn-beta
	fi
	sudo umount /media/usb128/sda
}

function image {
	sudo mount usb128.img /media/usb128/sda
	if [ "$TARGET" = "stable" ] ; then
		printf "[$BADGE] Writing stable kernel to usb128.img\n"
		sudo cp bin/kernel-stable /media/usb128/sda/boot/popcorn-stable
	else
		printf "[$BADGE] Writing beta kernel to usb128.img\n"
		sudo cp bin/kernel-beta /media/usb128/sda/boot/popcorn-beta
	fi
	sudo umount /media/usb128/sda
}

function writeimg {
	printf "[$BADGE] Writing local image to /dev/sda. This may take a while.\n"
	printf "    (Note: This may require a root password)\n"
	sudo umount /dev/sda
	sudo umount usb128.img
	printf "[$BADGE] Backing up...n"
	sudo dd if=/dev/sda of=usb128.img.backup
	printf "[$BADGE] Writing...\n"
	sudo dd if=usb128.img of=/dev/sda
}

function qemuimage {
	printf "[$BADGE] Qemu: i386, usb128.img\n"
	qemu-system-i386 -hda usb128.img --no-kvm
}

function qemudev {
	printf "[$BADGE] Qemu: i386, /dev/sda\n"
	sudo qemu-system-i386 -hda usb128.img --no-kvm
}

# Call the build process
main |& grep --color -e 'error\|errors\|warning'

 