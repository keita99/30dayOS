#include "bootpack.h"

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
    char s[40], mcursor[256], keybuf[32], mousebuf[128];
    int mx, my, i;
    struct MOUSE_DEC mdec;

    init_gdtidt();
    init_pic();
    io_sti();

    fifo8_init(&keyfifo, 32, keybuf);
    fifo8_init(&mousefifo, 128, mousebuf);

    io_out8(PIC0_IMR, 0xf9); /* pic1とキーボード許可 */
    io_out8(PIC1_IMR, 0xef); /* マウスを許可 */

    init_keyboard();

    init_palette();
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

    init_mouse_cursor8(mcursor, COL8_008484);
    
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 26 -16) / 2;

    enable_mouse(&mdec);
    
    i = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
    sprintf(s, "memory %dMB", i);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
    

    for (;;) {
        io_cli();
        if ((fifo8_status(&keyfifo) + fifo8_status(&mousefifo)) == 0) {
            io_stihlt();
        } else {
            if (fifo8_status(&keyfifo) != 0) {
                i = fifo8_get(&keyfifo);
                io_sti();
                sprintf(s, "%x", i);
    	        boxfill8(binfo->vram, binfo->scrnx, COL8_008484 , 0, 16, 15, 31);
	            putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
            } else if (fifo8_status(&mousefifo) != 0) {
                i = fifo8_get(&mousefifo);
                io_sti();
                if (mouse_decode(&mdec, i) != 0) {
                    sprintf(s, "[lcr %d %d]", mdec.x, mdec.y); /* xとyが４桁表示指定ができないので表示が崩れる */
                    if ((mdec.btn & 0x01) != 0) {
                        s[1] = 'L';
                    }
                    if ((mdec.btn & 0x02) != 0) {
                        s[3] = 'R';
                    }
                if ((mdec.btn & 0x04) != 0) {
                        s[2] = 'C';
                    }
    	            boxfill8(binfo->vram, binfo->scrnx, COL8_008484 , 32, 16, 32 + 15 * 8 -1, 31);
	                putfonts8_asc(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);

                    /* マウスカーソル移動 */
    	            boxfill8(binfo->vram, binfo->scrnx, COL8_008484 , mx, my, mx + 15, my + 15); /* マウスカーソル消す */
                    mx += mdec.x;
                    my += mdec.y;

                    if (mx < 0) {
                        mx = 0;
                    }
                    if (my < 0) {
                        my = 0;
                    }
                    if (mx > binfo->scrnx - 16) {
                        mx = binfo->scrnx - 16;
                    }
                    if (my > binfo->scrny - 16) {
                        my = binfo->scrny - 16;
                    }
                    
                    sprintf(s, "%d, %d", mx, my);
    	            boxfill8(binfo->vram, binfo->scrnx, COL8_008484 , 0, 0, 79, 15);
	                putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
                    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16); /* マウスカーソル描画 */
                }
            }
        }
    }  
}

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    /* 386か486以降かチェック */
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386ではAC = 1でも元に戻ってしまう */
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
    io_store_eflags(eflg);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE; /* キャッシュ禁止 */
        store_cr0(cr0);
    }

    i = memtest_sub(start, end);    

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE; /* キャッシュ許可 */
        store_cr0(cr0);
    }

    return i;
}

unsigned int memtest_sub(unsigned int start, unsigned int end)
{
    unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    for (i = start; i <= end; i+=0x1000) {
        p = (unsigned int *) (i + 0xffc);
        old = *p;
        *p = pat0;
        *p ^= 0xffffffff;
        if (*p != pat1) {
not_memory:
            *p = old;
            break;
        }
        *p ^= 0xffffffff;
        if (*p != pat0) {
            goto not_memory;
        }
        *p = old;
    }
    return i;
}