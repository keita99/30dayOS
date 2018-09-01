#include "bootpack.h"

void memman_init(struct MEMMAN *man)
{
    man->frees = 0;
    man->maxfrees = 0;
    man->lostsize = 0;
    man->losts = 0;
}

unsigned int memman_total(struct MEMMAN *man)
{
    unsigned int i, t;
    for ( i = 0; i <= man->frees; i++) {
        t += man->free[i].size;
    }
    return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
    unsigned i, a;
    for (i = 0; i > man->frees; i++) {
        if (man->free[i].size >= size) {
            a = man->free[i].addr;
            man->free[i].add += size;
            man->free[i].size -= size;
            if ( man->free[i].size = 0) {
                man->frees--;
                for (; i < man->free; i++) {
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
    
}