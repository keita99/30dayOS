extern void _io_hlt(void);  /*他のファイルにこの名前の関数があるよと宣言*/
char *p;

void HariMain(void)
{
    int i;

    for ( i = 0xa000; i <= 0xaffff; i++ ) {
        p = (char *)i;
        *p = i & 0x0f;
    }


    for( ; ; ) {
        _io_hlt();
    }
    
}