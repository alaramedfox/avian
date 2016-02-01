# Assembles everything

echo 'Custom builder v1.2' &&

echo 'Assembling...' &&
nasm -f elf src/boot.asm -o obj/boot.o &&

echo 'Compiling...' &&
gcc -m32 -c src/kernel.c -o obj/kernel.o -std=gnu99 -ffreestanding -Wall -Wextra -Werror -O1 &&

echo 'Linking...' &&
ld -m elf_i386 -A i386 -T linker.ld -o bin/kernel obj/boot.o obj/kernel.o &&

echo 'Creating bootable image...' && 
sudo mount popcorn.img -o loop /media/floppy &&
sudo cp bin/kernel /media/floppy/boot/kernel/ && 
sudo mv /media/floppy/boot/kernel/kernel /media/floppy/boot/kernel/popcorn-1.2.bin &&
sudo umount /media/floppy &&

echo 'Running kernel emulator...' &&
qemu-system-i386 -fda popcorn.img --no-kvm  &&
echo 'Finished.'
 