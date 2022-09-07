Jinux: kernel.o boot.o
	i686-elf-g++ -T linker.ld -o Jinux.bin -ffreestanding -O2 -nostdlib boot.o kernel.o
kernel.o: kernel.cpp
	i686-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
boot.o: boot.s
	i686-elf-as boot.s -o boot.o

clean: 
	rm *.o

install: Jinux 
	mkdir -p isodir/boot/grub
	cp Jinux.bin isodir/boot/Jinux.bin
	cp grub.cfg isodir/boot/grub
	grub-mkrescue -o Jinux.iso isodir