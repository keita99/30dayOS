void _io_hlt(void);  /*他のファイルにこの名前の関数があるよと宣言*/

void HariMain(void)
{
fin:
    _io_hlt();
    goto fin;
    
}