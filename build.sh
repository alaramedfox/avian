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

CFLAGS=" -m32 -ffreestanding -fno-exceptions -std=c99 "
CINC=" -Ilib/include -Icore/include "
CWARN=" "

SOURCES=(". lib core")

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
	HEADER="lib/include/buildcount.h"

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
	increment_build "1"
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
				printf "$INFO Compiling $SRC\n"
				colorgcc -c $DIR/$NAME.c -o $OBJ/$NAME.o $CINC $CFLAGS $CWARN
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
		ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel-alpha obj/*.o
		NEWSIZE=$(stat -c%s "bin/kernel-alpha")
		
		printf "$INFO Generated binary bin/kernel-alpha ($NEWSIZE bytes)\n"
		printf "$INFO (Net change: $(( $NEWSIZE - $OLDSIZE )) bytes)\n"
		#return
	#done
}

function run {
	printf "$INFO Executing kernel-alpha with QEMU...\n"
	qemu-system-i386 -kernel bin/kernel-alpha -fda test.img --no-kvm
	return
}


main $@


 