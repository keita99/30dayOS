; haribote-os
;

; BOOT_INFO
CYLS    EQU     0x0ff0          ; ブートセクタ設定
LEDS    EQU     0x0ff1
VMODE   EQU     0x0ff2          ;色数情報　何ビットカラーか？
SCRNX   EQU     0x0ff4          ;解像度 X
SCRNY   EQU     0x0ff6          ;解像度 Y
VRAM    EQU     0x0ff8          ;グラフィックバッファ開始番地

        ORG     0xc200          ;プログラムが読み込まれる場所

        MOV     AL,0x13         ; VGAグラフィックス　320x300x8bit カラー
        MOV     AH,0x00
        INT     0x10
        MOV     BYTE [VMODE],8
        MOV     WORD [SCRNX],320
        MOV     WORD [SCRNY],200
        MOV     DWORD [VRAM],0x000a0000

;キーボードのLEDをBIOSに聞く
        MOV     AH,0x02
        INT     0x16
        MOV     [LEDS],AL


fin:
        HLT
        JMP fin