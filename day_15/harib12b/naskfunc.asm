;asminthandler27などまだ

; naskfunc

    GLOBAL  io_hlt, io_cli, io_sti, io_stihlt   ; 関数宣言
    GLOBAL  io_in8, io_in16, io_in32
    GLOBAL  io_out8, io_out16, io_out32
    GLOBAL  io_load_eflags, io_store_eflags
    GLOBAL  load_gdtr, load_idtr
    GLOBAL  asm_inthandler21, asm_inthandler27, asm_inthandler2c, asm_inthandler20
    EXTERN  inthandler21, inthandler27, inthandler2c, inthandler20
    GLOBAL  load_cr0, store_cr0
    GLOBAL  load_tr, taskswitch4, taskswitch3
    GLOBAL  memtest_sub

[SECTION .text]

io_hlt:    ; void io_hlt(void)
    HLT
    RET

io_cli:     ; void io_cli(void)
    CLI
    RET

io_sti:     ; void io_sti(void)
    STI
    RET

io_stihlt:  ; void io_stihlt(void)
    STI
    HLT
    RET

io_in8:     ;int io_in8(int port)
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,0
    IN      AL,DX
    RET

io_in16:     ;int io_in16(int port)
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,0
    IN      AX,DX
    RET

io_in32:     ;int io_in32(int port)
    MOV     EDX,[ESP+4]     ; port
    IN      EAX,DX
    RET

io_out8:     ;int io_out8(int port, int data)
    MOV     EDX,[ESP+4]     ; port
    MOV     AL,[ESP+8]      ; data
    OUT     DX,AL
    RET

io_out16:     ;int io_out16(int port, int data)
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,[ESP+8]      ; data
    OUT     DX,AX
    RET

io_out32:     ;int io_out32(int port, int data)
    MOV     EDX,[ESP+4]     ; port
    MOV     EAX,[ESP+8]      ; data
    OUT     DX,EAX
    RET

io_load_eflags: ; int io_load_eflags(void)
    PUSHFD
    POP     EAX
    RET

io_store_eflags: ; int io_store_eflags(int eflags)
    MOV     EAX,[ESP+4]
    PUSH    EAX
    POPFD
    RET

load_gdtr:  ; void load_gdtr(int limit, int addr);
    MOV     AX,[ESP+4]
    MOV     [ESP+6],AX
    LGDT    [ESP+6]
    RET

load_idtr:  ; void load_idtr(int limit, int addr);
    MOV     AX,[ESP+4]
    MOV     [ESP+6],AX
    LIDT    [ESP+6]
    RET

asm_inthandler21:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler21
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD

asm_inthandler27:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler27
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD    
    
asm_inthandler2c:
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler2c
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD    

asm_inthandler20: ;PIT(タイマー割り込み))
    PUSH    ES
    PUSH    DS
    PUSHAD
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler20
    POP     EAX
    POPAD
    POP     DS
    POP     ES
    IRETD    

load_cr0:   ;int load_cr0(void);
    MOV     EAX,CR0
    RET

store_cr0:   ;void store(int cr0);
    MOV     EAX,[ESP+4]
    MOV     CR0,EAX
    RET

load_tr:    ;void load_tr(int tr);
    LTR     [ESP+4]
    RET

 taskswitch4:;void taskswitch4(void);
    JMP     4*8:0
    RET

taskswitch3:;void taskswitch3(void);
    JMP     3*8:0
    RET

memtest_sub: ;unsigned int memtest_sub(unsigned int start, unsigned int end);
    PUSH    EDI
    PUSH    ESI
    PUSH    EBX
    MOV     ESI,0xaa55aa55
    MOV     EDI,0x55aa55aa
    MOV     EAX,[ESP+12+4]
mts_loop:
    MOV     EBX,EAX
    ADD     EBX,0xffc
    MOV     EDX,[EBX]
    MOV     [EBX],ESI
    XOR     DWORD [EBX],0xffffffff
    CMP     EDI,[EBX]
    JNE     mts_fin
    MOV     [EBX],EDX
    ADD     EAX,0x1000
    CMP     EAX,[ESP+12+8]
    JBE     mts_loop
    POP     EBX
    POP     ESI
    POP     EDI
    RET
mts_fin:
    MOV     [EBX],EDX
    POP     EBX
    POP     ESI
    POP     EDI
    RET