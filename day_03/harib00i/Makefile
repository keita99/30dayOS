run: haribote.img
	qemu-system-i386 -fda $^

ipl10.bin: ipl10.asm Makefile
	nasm ipl10.asm -o ipl10.bin -l ipl10.lst

asmhead.bin: asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.bin -l asmhead.lst

bootpack.hrb: bootpack.c har.ld Makefile       # Cファイルをリンカスクリプトを用いてコンパイル
	gcc -march=i486 -m32 -nostdlib -T har.ld bootpack.c -o bootpack.hrb


haribote.img: ipl10.bin asmhead.bin
	echo asmhead.bin > asmhead.name
	dd if=ipl10.bin		of=haribote.img count=2880 bs=512 conv=notrunc
	dd if=asmhead.name	of=haribote.img count=1 bs=512 seek=19 conv=notrunc
	dd if=asmhead.bin	of=haribote.img count=1 bs=512 seek=33 conv=notrunc

img:
	make -r haribote.img

debug:
	qemu-system-i386 -fda haribote.img -gdb tcp::10000 -S

clean:
	rm -rf *.img *.lst *.sys *.name *.bin