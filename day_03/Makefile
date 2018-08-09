run: helloos.img
	qemu-system-i386 -fda $^

ipl.bin: ipl.asm
	nasm ipl.asm -o ipl.bin -l ipl.lst

helloos.img: ipl.bin #haribote.sys
#	echo haribote.sys > haribote.name
	dd if=ipl.bin		of=helloos.img count=2880 bs=512 conv=notrunc
#	dd if=haribote.name	of=helloos.img count=1 bs=512 seek=19 conv=notrunc
#	dd if=haribote.sys	of=helloos.img count=1 bs=512 seek=33 conv=notrunc

clean:
	rm -rf *.img *.lst