; haribote-os
;

        ORG     0xc200          ;プログラムが読み込まれる場所

        MOV     AL,0x13         ; VGAグラフィックス　320x300x8bit カラー
        MOV     AH,0x00
        INT     0x10

fin:
        HLT
        JMP fin