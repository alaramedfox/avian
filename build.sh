# Assembles everything

echo 'Custom builder v1.1' &&

echo 'Assembling...' &&
nasm -f elf src/boot.asm -o obj/boot.o &&

echo 'Compiling...' &&
gcc -m32 -c src/kernel.c -o obj/kernel.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror -O1 &&

echo 'Linking...' &&
ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel obj/boot.o obj/kernel.o &&

echo 'Running kernel emulator...' &&
qemu-system-i386 -kernel bin/kernel -no-kvm &&
echo 'Finished.'
 