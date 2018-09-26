; haribote-ipl
; TAB=4
; os自作入門のusbブート - Qiita https://qiita.com/phoneticdata/items/9b0c74f59aba9c0a2ed1
; qemu-system-x86_64 -usb にて起動する


[bits 16]
[ORG 0x7c00]
        ;; BPB Structure                                                                                                                     
JMP     entry           ;BS_jmpBoot
NOP                                                                                                  
BS_OEMName      DB "HARIBOTE"
BPB_BytsPerSec  DW 0x0200
BPB_SecPerClus  DB 0x01
BPB_RsvdSecCnt  DW 0x0001
BPB_NumFATs     DB 0x02
BPB_RootEntCnt  DW 0x0000
BPB_TotSec16    DW 0x0000
BPB_Media       DB 0xf8
BPB_FATSz16     DW 0x0000
BPB_SecPerTrk   DW 0xffff
BPB_NumHeads    DW 0x0001
BPB_HiDDSec     DD 0x00000000
BPB_TotSec32    DD 0x00ee5000
BPB_FATSz32     DD 0x000000ed
BPB_ExtFlags    DW 0x0000
BPB_FSVer       DW 0x0000
BPB_RootClus    DD 0x00000000
BPB_FSInfo      DW 0x0001
BPB_BkBootSec   DW 0x0000
times   12      DB 0    ;BPB_Reserverd                                                                                               
BS_DrvNum       DB 0x80
BS_Reserved1    DB 0x00
BS_BootSig      DB 0x29
BS_VolID        DD 0xa0a615c
BS_VolLab       DB "ISHIHA BOOT"
BS_FileSysType  DB "FAT32   "



entry:  
    CLI

    MOV AX, 0
    MOV DS, AX
    MOV SS, AX
    MOV ES, AX
    MOV BX, AX
    MOV CX, AX
    MOV SP, 0x7c00
prepare:
    STI
    MOV [drv], DL

    MOV AH, 0x41                ; Set Function 0x41
    MOV word bx, 0x55aa          
    INT 0x13                   ; Call Interupt
    JC unsupported        ; If Extentions aren't Supported, Jump




   xor ax, ax
   add ax, 1                   ; clear carry flag
   XOR EDI, EDI

   MOV AH, 0x45
   MOV AL, 0x01
   MOV DL, 0x80
   INT 0x13

   MOV AX, 0
   MOV DS, AX

loop:
    MOV CL, 0
retry:
    PUSH DS
    PUSHAD
    MOV DL, [drv]
    MOV AH, 0x42
    MOV AL, 0x00
    MOV SI, DAPS
    INT 0x13
    JNC next

    ADD CL, 1
    MOV DL, 0x80 
    MOV AH, 0x00
    INT 0x13
    CMP CL, 6
    JAE error
    JMP retry
next:

    XOR EAX, EAX
    XOR EBX, EBX
    XOR ECX, ECX

    ADD EDI, 1
    MOV ECX, lba0
    MOV [ECX], EDI

    XOR EAX, EAX
    XOR ECX, ECX
    XOR EBP, EBP

    MOV AX, [addr]
    MOV ECX, addr
    MOV EBX, segm
    ADD AX, 0x200
    ADC BP, 0
    SHL BP, 12
    ADD BP, [segm]
    MOV [EBX], BP


    MOV [ECX], AX
    MOV [EBX], BP

    CMP EDI, 0x16a
    JB loop

    MOV ECX, 0xc200

    MOV EAX, 0x0000
    MOV EBX, EAX
    MOV EDX, EAX
    MOV EBP, EAX
    MOV ESI, EAX
    MOV EDI, EAX

    MOV CH, 10
    MOV [0x0ff0], CH

    MOV ECX, EAX
    JMP 0x0000:0xc200
error:
    POPAD
    POP DS
    MOV     SI,msg
putloop:
    MOV     AL,[SI]
    ADD     SI,1            
    CMP     AL,0
    JE      fin
    MOV     AH,0x0e         
    MOV     BX,0       
    INT     0x10           
    JMP     putloop
fin:
    HLT                     
    JMP     fin            
msg:
    DB      0x0a, 0x0a      
    DB      "load error"
    DB      0x0a            
    DB      0
msg1:
    DB      0x0a, 0x0a      
    DB      "not supported"
    DB      0x0a            
    DB      0

unsupported:
    MOV SI, msg1
    JMP putloop

drv:    DB 0x80
DAPS:   DB 0x10               ; Size of Structure (16 bytes, always this for DAPS)
        DB 0                  ; Always 0
        DB 1                  ; Number of Sectors to Read (1x512)
        DB 0                  ; Always 0 
addr:   DW 0x8000             ; Target Location for Reading To (0x8000 = 0x0800:0x0000)  
segm:   DW 0x0000             ; Page Table (0, Disabled)
lba0:   DD 1                  ; Read from 2nd block (code I want to load)
        DD 0                  ; Large LBAs, dunno what this does



    RESB    0x01fe-($-$$)       
    DW 0AA55h