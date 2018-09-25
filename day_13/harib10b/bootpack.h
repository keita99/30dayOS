/* asmhead.asm */
struct BOOTINFO {
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};
#define ADR_BOOTINFO 0x00000ff0

/* naskfunc.asm */
extern void io_hlt(void);  /*他のファイルにこの名前の関数があるよと宣言*/
extern void io_cli(void);
extern void io_sti(void);
extern void io_stihlt(void);
extern void io_out8(int port, int data);
extern int io_in8(int port);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);
extern void load_gdtr(int limit, int addr);
extern void load_idtr(int limit, int addr);
extern void asm_inthandler21(void);
extern void asm_inthandler27(void);
extern void asm_inthandler2c(void);
extern void asm_inthandler20(void);
extern int load_cr0(void);
extern void store_cr0(int cr0);
extern int memtest_sub(int start, int end);


/* mysprintf.c */
extern void sprintf (char *str, char *fmt, ...);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void init_screen8(char *vram, int x, int y);

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);

#define COL8_000000     0
#define COL8_FF0000     1
#define COL8_00FF00     2
#define COL8_FFFF00     3
#define COL8_0000FF     4
#define COL8_FF00FF     5
#define COL8_00FFFF     6
#define COL8_FFFFFF     7
#define COL8_C6C6C6     8
#define COL8_840000     9
#define COL8_008400     10
#define COL8_848400     11
#define COL8_000084     12
#define COL8_840084     13
#define COL8_008484     14
#define COL8_848484     15


/* dsctbl.c */
struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32    0x008e


/* int.c */
void init_pic(void);
void inthandler27(int *esp);

#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1


/* keyboard.c */
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(void);

#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD     0x0064
extern struct FIFO8 keyfifo;

/* mouse.c */
struct MOUSE_DEC {
    unsigned char buf[3], phase;
    int x, y, btn;
};

void inthandler2c(int *esp);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned dat);

extern struct FIFO8 mousefifo;


/* fifo.c */
struct FIFO8 {
	unsigned char *buf;
    int p, q, size, free ,flags;
};

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

/* memory.c */
#define MEMMAN_FREES    4090 /* 約32KB */
#define MEMMAN_ADDR     0x003c0000

struct FREEINFO {
    unsigned int addr,size;
};

struct MEMMAN {
    int frees, maxfrees, lostsize, losts;
    struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
unsigned int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);


/* sheet.c */
#define MAX_SHEETS  256

struct SHEET {
    struct SHTCTL *ctl;    
    unsigned char *buf;
    int bxsize, bysize, vx0, vy0, col_inv, height, flags;
};

struct SHTCTL {
    unsigned char *vram, *map;
    int xsize, ysize, top;
    struct SHEET *sheets[MAX_SHEETS];
    struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);


/* timer.c */
#define MAX_TIMER 500

struct TIMER {
    unsigned int timeout, flags;
    struct FIFO8 *fifo;
    unsigned char data;
};
struct TIMERCTL {
    unsigned int count, next, using;
    struct TIMER *timers[MAX_TIMER];
    struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;

void init_pit(void);
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void inthandler20(int *esp);
void timer_settime(struct TIMER *timer, unsigned int timeout);