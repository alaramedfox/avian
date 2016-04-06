#!/bin/bash
set -e

GRN='\033[0;32m'
RED='\033[0;31m'
BLK='\033[0m'

INFO="${GRN}Info:${BLK}"
WARN="${RED}Warning:${BLK}"

VERSION="Avian Builder Tool v5.0"
OPTIONS=()
TARGET=()
ROOT="."
OBJ="obj"
BIN="bin"
IMAGE="test.img"

CFLAGS=" -march=pentium3 -m32 -ffreestanding -fno-exceptions -std=c99 -nostdlib "
CINC=" -Isrc/include -Isrc/asm "
CWARN=" -Wall -Wextra -Werror -Wfatal-errors -Wno-unused "
#CWARN=" -Wfatal-errors "

QFLAGS=" -ctrl-grab -fda $IMAGE -m 16 -d cpu_reset "

SOURCES=(" src src/asm src/lex ")

function increment_file {
	file=$1
	old="`sed  's/^ *//' $file` +1"  
	echo $old | bc > $file.temp  
	mv $1.temp $file
	return "`sed  's/^ *//' $1`"
}

function increment_build {
	BUILDFILE="version/build"
	MAJORFILE="version/major"
	TIMEFILE="version/time"
	HEADER="src/include/buildcount.h"

	version="`sed  's/^ *//' $MAJORFILE`"  
	old="`sed  's/^ *//' $BUILDFILE` +1"  
	echo $old | bc > $BUILDFILE.temp  
	mv $BUILDFILE.temp $BUILDFILE
	
	echo "$version`sed  's/^ *//' $BUILDFILE` - `date`" > $TIMEFILE
	echo "#define BUILD \"`sed  's/^ *//' $BUILDFILE`\"" > $HEADER 
	echo "#define TIMESTAMP \"$version`sed  's/^ *//' $BUILDFILE` - `date`\"" >> $HEADER  
	echo "#define VERSION \"$version`sed  's/^ *//' $BUILDFILE`\"" >> $HEADER
}

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
			"-write") OPTIONS+=" -write " ;;
			"-run") OPTIONS+=" -run " ;;
			"-dev") OPTIONS+=" -dev " ;;
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
			"-write") update ;;
			"-run") run ;;
			"-dev") DEV="1" ;;
			*) printf "$WARN Unknown option '$i'\n" ;;
		esac
	done
}

function make_all {
	increment_build "1"
	TARGET+=$(ls -R | grep "\.c\|\.asm")
	assemble
	compile
	link
	sudo mount -o loop bin/kernel.img /media/floppy
	sudo cp bin/kernel-alpha /media/floppy/boot/avian.bin
	sudo umount /media/floppy
}

function assemble {
	for i in $TARGET; do
		NAME=`echo "$i" | cut -d'.' -f1`
		if [ -f src/asm/$NAME.asm ]; then
			printf "$INFO Assembling $i\n"
			nasm -f elf src/asm/$NAME.asm -o $OBJ/$NAME.o
		fi
	done
}

function compile {
	for DIR in $SOURCES; do
		for SRC in $TARGET; do
			NAME=`echo "$SRC" | cut -d'.' -f1`
			if [ -f $DIR/$NAME.c ]; then
				printf "$INFO Compiling $SRC\n"
				gcc -c $DIR/$NAME.c -o $OBJ/$NAME.o $CINC $CFLAGS $CWARN
			fi
		done
	done
}

function link {
	printf "$INFO Linking object files:\n"
	ls obj/
	printf "\n"
	#$for i in $TARGET; do
		if [ -f "bin/kernel-alpha" ]; then
			OLDSIZE=$(stat -c%s "bin/kernel-alpha")
		else
			OLDSIZE=0
		fi
		#BINARY=`echo $i | grep "alpha\|beta\|stable"`
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-alpha obj/*.o -Map kernel.map
		NEWSIZE=$(stat -c%s "bin/kernel-alpha")
		
		printf "$INFO Generated binary bin/kernel-alpha ($NEWSIZE bytes)\n"
		printf "$INFO (Net change: $(( $NEWSIZE - $OLDSIZE )) bytes)\n"
		#return
	#done
}
LOCAL="1"
function update {
   LOCAL="0"
   cp bin/kernel.img $IMAGE
}

function run {
   if [ $LOCAL = "1" ]; then
      printf "$INFO Executing kernel-alpha with QEMU...\n"
      qemu-system-i386 -kernel bin/kernel-alpha $QFLAGS
   else
	   printf "$INFO Executing grub-test.img with QEMU...\n"
	   qemu-system-i386 $QFLAGS
	fi
	return
}


main $@


 