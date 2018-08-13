extern void _io_hlt(void);  /*他のファイルにこの名前の関数があるよと宣言*/
extern void _write_mem8(int addr, int data);

void HariMain(void)
{
    int i;

    for ( i = 0xa000; i <= 0xaffff; i++ ) {
        _write_mem8( i, i & 0x0f);
    }


    for( ; ; ) {
        _io_hlt();
    }
    
}