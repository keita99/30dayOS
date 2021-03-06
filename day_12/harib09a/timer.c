#include "bootpack.h"

#define PIT_CTRL    0x0043
#define PIT_CNT0    0x0040

void init_pit(void)
{
    io_out8(PIT_CTRL,0x34);
    io_out8(PIT_CNT0,0x9c); /* 割り込み周期 11932回（１秒間に１００回割り込み)(0x2e9c) */
    io_out8(PIT_CNT0,0x2e);
}

void inthandler20(int *esp)
{
    io_out8(PIC0_OCW2, 0x60);

    return;
}
