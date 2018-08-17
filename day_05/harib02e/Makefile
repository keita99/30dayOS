run: haribote.img
	qemu-system-i386 -fda $^

ipl10.o: ipl10.asm Makefile
	nasm ipl10.asm -o ipl10.o -l ipl10.lst

asmhead.o: asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.o -l asmhead.lst

naskfunc.o: naskfunc.asm Makefile
	nasm -g -f elf naskfunc.asm -o naskfunc.o -l naskfunc.lst

hankaku2c: hankaku2c.c #hankaku.txtをC言語形式に変換するツールのコンパイル
	gcc hankaku2c.c -o hankaku2c
	chmod 755 hankaku2c

hankaku.c: hankaku.txt hankaku2c	#hankaku.txtをhankaku.cに変換
	./hankaku2c

bootpack.hrb: bootpack.c naskfunc.o hankaku.c har.ld Makefile       # Cファイルをリンカスクリプトを用いてコンパイル
	gcc -march=i486 -m32 -nostdlib -T har.ld bootpack.c naskfunc.o hankaku.c -o bootpack.hrb

haribote.sys: asmhead.o bootpack.hrb Makefile
	cat asmhead.o bootpack.hrb > haribote.sys

haribote.img: ipl10.o haribote.sys Makefile
	mformat -f 1440 -C -B ipl10.o -i haribote.img ::
	mcopy haribote.sys -i haribote.img ::

#	echo haribote.sys > haribote.name
#	dd if=ipl10.o		of=haribote.img count=2880 bs=512 conv=notrunc
#	dd if=haribote.name	of=haribote.img count=1 bs=512 seek=19 conv=notrunc
#	dd if=haribote.sys	of=haribote.img count=1 bs=512 seek=33 conv=notrunc

img:
	make -r haribote.img

debug:
	qemu-system-i386 -fda haribote.img -gdb tcp::10000 -S

clean:
	rm -rf *.img *.lst *.sys *.name *.o *.hrb