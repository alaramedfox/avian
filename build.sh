#!/bin/bash
set -e

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

INFO="${GRN}Info:${BLK}"
WARN="${RED}Warning:${BLK}"

VERSION="Popcorn Builder Tool v4.0"
OPTIONS=()
TARGET=()
ROOT="."
OBJ="obj"
BIN="bin"

SOURCES=(". lib core")


function printhelp {
	printf "\n
$VERSION\n
Usage: build -option [files]\n
Options:\n
-c    	Compile C source code in /src/core\n
-c-all	Compile everything in /src/core\n
-a    	Assemble ASM source code in /src\n
-m			Compile the main.c file in /src\n
-l			Link together all files in /obj\n
-make		Assemble, compile, and link Everything\n
-h    	Display this help text\n"
}

function main {
	for i in $@; do
		case "$i" in
			"-c") OPTIONS+=" -c " ;;
			"-a") OPTIONS+=" -a " ;;
			"-h") OPTIONS+=" -h " ;;
			"-l") OPTIONS+=" -l " ;;
			"-make") OPTIONS+=" -make " ;;
			"-run") OPTIONS+=" -run " ;;
			*) TARGET+=" $i " ;;
		esac
	done
	
	for i in $OPTIONS; do
		case $i in
			"-c") compile;;
			"-a") assemble ;;
			"-h") printhelp ;;
			"-l") link ;;
			"-make") make_all;;
			"-run") run ;;
			*) printf "$WARN Unknown option '$i'\n" ;;
		esac
	done
}

function make_all {
	TARGET+=$(ls -R | grep "\.c\|\.asm")
	assemble
	compile
	link
}

function assemble {
	for i in $TARGET; do
		NAME=`echo "$i" | cut -d'.' -f1`
		if [ -f $NAME.asm ]; then
			printf "$INFO Assembling $i\n"
			nasm -f elf $NAME.asm -o $OBJ/$NAME.o
		fi
	done
}

function compile {
	for DIR in $SOURCES; do
		for SRC in $TARGET; do
			NAME=`echo "$SRC" | cut -d'.' -f1`
			if [ -f $DIR/$NAME.c ]; then
				printf "$INFO Compiling $SRC...\n"
				colorgcc -m32 -c $DIR/$NAME.c -o $OBJ/$NAME.o --sysroot=$ROOT -Ilib/include/ -Icore/include/ -fno-exceptions -std=c99 -ffreestanding
			fi
		done
	done
}

function link {
	printf "$INFO Linking object files:\n"
	ls obj/
	printf "\n"
	for i in $TARGET; do
		BINARY=`echo $i | grep "alpha\|beta\|stable"`
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-$BINARY obj/*.o
		NEWSIZE=$(stat -c%s "bin/kernel-$BINARY")
		printf "$INFO Generated binary bin/kernel-$BINARY ($NEWSIZE bytes)\n"
		#return
	done
}

function run {
	for i in $TARGET; do
		BINARY=`echo $i | grep "alpha\|beta\|stable"`
		printf "$INFO Executing kernel-$BINARY with QEMU...\n"
		qemu-system-i386 -kernel bin/kernel-$BINARY --no-kvm
		return
	done
}


main $@


 