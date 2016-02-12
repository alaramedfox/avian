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
DIR="src/core/"

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
			"-c-all") OPTIONS+=" -c-all " ;;
			"-m") OPTIONS+=" -m " ;;
			"-make") OPTIONS+=" -make " ;;
			*) TARGET+=" $i " ;;
		esac
	done
	
	for i in $OPTIONS; do
		case $i in
			"-c") compile;;
			"-a") assemble ;;
			"-h") printhelp ;;
			"-l") link ;;
			"-c-all") compile_all;;
			*) printf "$WARN Unknown option '$i'\n" ;;
		esac
	done
}

function assemble {
	printf "$INFO Assembling\n"
	nasm -f elf src/boot.asm -o obj/boot.o
}

function compile_all {
	FILES=$(ls src/ -R | grep '\.c')
	for i in $FILES; do
		NAME=`echo "$i" | cut -d'.' -f1`
		printf "$INFO Compiling $i...\n"
		if [ "$i" = "main.c" ]; then
			DIR="src/"
		else
			DIR="src/core/"
		fi
		colorgcc -m32 -c $DIR$NAME.c -o obj/$NAME.o --sysroot=src/ -Isrc/include/ -fno-exceptions -O1 -std=c99 -ffreestanding
	done
}

function compile {
	for i in $TARGET; do
		NAME=`echo "$i" | cut -d'.' -f1`
		printf "$INFO Compiling $i...\n"
		if [ "$i" = "main.c" ]; then
			DIR="src/"
		else
			DIR="src/core/"
		fi
		colorgcc -m32 -c $DIR$NAME.c -o obj/$NAME.o --sysroot=src/ -Isrc/include/ -fno-exceptions -O1 -std=c99 -ffreestanding
	done
}

function link {
	printf "$INFO Linking object files:\n"
	ls obj/
	printf "\n"
	for i in $TARGET; do
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-$i obj/*.o
		printf "$INFO Generated binary bin/kernel-$i\n"
	done
}


main $@


 