#include "bootpack.h"

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


void memman_init(struct MEMMAN *man)
{
    man->frees = 0;
    man->maxfrees = 0;
    man->lostsize = 0;
    man->losts = 0;
    return;
}

unsigned int memman_total(struct MEMMAN *man)
{
    unsigned int i, t;
    for ( i = 0; i < man->frees; i++) {
        t += man->free[i].size;
    }
    return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
    unsigned i, a;
    for (i = 0; i < man->frees; i++) {
        if (man->free[i].size >= size) {
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;
            if ( man->free[i].size == 0) {
                man->frees--;
                for (; i < man->frees; i++) {
                    man->free[i] = man->free[i + 1];
                }
            }
            return a;
        }
    }
    return 0;
}

unsigned int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i,j;

    /* addr順に並べるように入れるところを探す */
    for (i = 0; i < man->frees; i++) {
        if (man->free[i].addr > addr) {
            break;
        }
    }

    if (i > 0) {
        if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
            /* 前の空き領域にまとめる */
            man->free[i - 1].size += size;
            if (i < man->frees) {
                if (addr + size == man->free[i].addr) {
                    /* 後ろの空き領域もまとめられる */
                    man->free[i - 1].size += man->free[i].size;
                    /* 後ろを詰める */
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];
                    }
                }
            }
            return 0;
        }
    }    

    if (i < man->frees) {
        if (addr + size == man->free[i].addr) {
            /* 後ろの空き領域とまとめられる */
            man->free[i].addr = addr;
            man->free[i].size += size;
            return 0;
        }
    }

    /* 前も後ろも空き領域にまとめられない時 */
    if (man->frees < MEMMAN_FREES) {
        /* free[i]より後ろをずらして空ける */    
        for (j = man->frees; j > i; j--){
            man->free[j] = man->free[j - 1];
        }        
        man->frees++;
        if (man->maxfrees < man->frees) {
            man->maxfrees = man->frees; /* 管理領域している最大数を増やす */
        }
        man->free[i].addr = addr;
        man->free[i].size = size;
        return 0;
    }

    man->losts++;
    man->lostsize += size;
    return -1; /* エラー終了 */
}

/* 4KB単位でのメモリ確保 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{
    unsigned int a;
    size = (size + 0xfff) & 0xfffff000;
    a = memman_alloc(man, size);
    return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i;
    size = (size + 0xfff) & 0xfffff000;
    i = memman_free(man, addr, size);
    return i;
}