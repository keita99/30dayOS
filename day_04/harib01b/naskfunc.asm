; naskfunc

    GLOBAL  _io_hlt,_write_mem8   ; 関数宣言

_io_hlt:    ; void _io_hlt
    HLT
    RET

_write_mem8:    ;void write_mem8(int addr, int data);
    MOV     ECX,[ESP+4]
    MOV     AL,[ESP+8]
    MOV     [ECX],AL
    RET