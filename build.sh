# Assembles everything

echo 'Assembling...'
nasm -f elf boot.asm -o obj/boot.o

echo 'Compiling the C kernel...'
gcc -m32 -c kernel.c -o obj/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo 'Linking...'
gcc -m32 -T linker.ld -o bin/kernel.bin -ffreestanding -O2 -nostdlib obj/boot.o obj/kernel.o -lgcc

echo 'Creating bootable filesystem...'
cp bin/kernel.bin CD_root/kernel.bin
#echo 'Creating bootable ISO...'
#mkisofs -o output.iso \
#   -b isolinux/isolinux.bin -c isolinux/boot.cat \
#   -no-emul-boot -boot-load-size 4 -boot-info-table \
#   CD_root

qemu-system-i386 -kernel bin/kernel.bin -no-kvm
echo 'Finished.'
