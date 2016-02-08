#!/bin/bash
set -e

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

BADGE="${GRN} v3.2 ${BLK}"
WARN="${RED} WARNING ${BLK}"
R="${RED}*${BLK}"
VERSION="Popcorn Builder Tool v4.0"
OPTIONS=()
TARGET="beta"
FLAGS=()
DEVICE="/dev/sda1"
IMAGE="popcorn.img"

function printhelp {
	printf "\n"
	printf "$GRN--${VERSION}--$BLK\n"
	printf "Syntax:  'build TARGET [-OPTION] [FLAGS] ...\n"
	printf "General options:\n"
	printf " -h, --help        Print this help text and exit\n"
	printf " -f, --force       Continue despite any errors\n"
	printf " -i  [image]       Specify local image\n"
	printf " -d  [device]      Specify device in /dev/\n"
	printf "Compilation options:\n"
	printf " -c                Compile kernel to [target]\n"
	printf "     strict        Turn gcc warnings into errors\n"
	printf "     warnings      Enable all gcc warnings\n"
	printf "     fatal         Fail on first error\n"
	printf "Device options:\n"
	printf " -w  FLAGS         Update kernel [target] on device\n"
	printf "     image         Write kernel to an image (usb-grub.img)\n"
	printf "     device        Write kernel to a physical drive (/dev/sda1)\n"
	printf "     update        Write local image to physical drive\n"
	printf "Emulation options:\n"
	printf " -q  FLAGS         Execute [target] using QEMU\n"
	printf "     kernel        Run [target] directly, with no bootloader\n"
	printf "     local         Run QEMU from local image\n"
	printf "     external      Run QEMU from external device\n"
	printf "Valid targets: 'stable', 'beta' (default), and 'alpha'\n"
}

function main {
	for i in $@; do
		case "$i" in
			"-c") OPTIONS+=" -c " ;;
			"-w") OPTIONS+=" -w " ;;
			"-h") OPTIONS+=" -h " ;;
			"-q") OPTIONS+=" -q " ;;
			"stable") TARGET="stable" ;;
			"beta") TARGET="beta" ;;
			"alpha") TARGET="alpha" ;;
			*) FLAGS+=" $i " ;;
		esac
	done
	
	for i in $OPTIONS; do
		case $i in
			"-c") assemble && compile && link ;;
			"-w") writeimg ;;
			"-h") printhelp ;;
			"-q") runqemu ;;
			*) printf "Unknown option '$i'\n" ;;
		esac
	done
}

function fail {
	printf "[$RED FAIL $BLK] $1\n"
}

function assemble {
	printf "[${BADGE}] Assembling\n"
	nasm -f elf src/boot.asm -o obj/boot.o || exit
}

function compile {
	printf "[${BADGE}] Compiling\n"
	COMPILE=()
	for i in $FLAGS; do
		case $i in
			"strict") 
				printf "Compiling with strict errors\n"
				COMPILE+=" -Werror " ;;
			"warnings") 
				printf "Compiling with extra errors\n"
				COMPILE+=" -Wall -Wextra " ;;
			"fatal") 
				printf "Compilation will fail on first error\n"
				COMPILE+=" -Wfatal-errors " ;;
		esac
	done
	colorg++ -m32 -c src/main.c -o obj/kernel.o ${COMPILE[@]} -std=c++03 -ffreestanding || exit
}

function link {
	printf "[$BADGE] Linking $TARGET kernel\n"
	ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-$TARGET obj/boot.o obj/kernel.o || exit
}

function device {
	if [ -f $DEVICE ]; then
		sudo umount $DEVICE
		sudo mount $DEVICE /media/usb
		printf "[$BADGE] Writing $TARGET kernel to device\n"
		sudo cp "bin/kernel-$TARGET" "/media/usb/boot/popcorn-$TARGET"
		sudo umount /media/usb
	else
		printf "[$BADGE] External device is not plugged in\n"
	fi
}

function image {
	printf "[$BADGE] Writing $TARGET kernel to $IMAGE\n"
	sudo mount $IMAGE /media/usb
	sudo cp "bin/kernel-$TARGET" "/media/usb/boot/popcorn-$TARGET"
	sudo umount /media/usb
}

function update {
	printf "[$BADGE] Updating external device from image\n"
	sudo mount $IMAGE /media/usb
	sudo mount $DEVICE /media/floppy
	sudo cp "/media/usb/boot/popcorn-$TARGET" "/media/floppy/boot/popcorn-$TARGET"
	sudo umount /media/usb
	sudo umount /media/floppy
}

function writeimg {
	for i in $FLAGS; do
		case $i in
			"image") image ;;
			"device") device ;;
			"update") update ;;
		esac
	done
}

function runqemu {
	for i in $FLAGS; do
		case $i in
			"kernel") 
				printf "Running QEMU using bin/kernel-$TARGET\n"
				qemu-system-i386 -kernel bin/kernel-$TARGET --no-kvm ;;
			"local") 
				printf "Running QEMU using local image $IMAGE\n"
				#qemu-system-i386 -fda qemu.img -hda $IMAGE --no-kvm ;;
				qemu-system-i386 -fda popcorn.img --no-kvm;;
			"external")
				printf "Running QEMU using external device $DEVICE\n"
				qemu-system-i386 -hda $DEVICE --no-kvm ;;
		esac
	done
}

main $@


 