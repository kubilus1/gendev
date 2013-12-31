 /*
   

    One sample is roughly 81 clock cycles.

 */
#include <string.h>

//#pragma NOJTBOUND

union Address {
    struct {
        unsigned char l;
        unsigned char h;
    };
    unsigned int w;
};
typedef struct banked_addr_t {
    char filler;
    char bank;
    union Address addr; 
} banked_addr;
typedef struct pcm_record_t {
    int addr;
    char bank;
    char filler;
    int datalen;
    int filler2;
} pcm_record;


unsigned long addr;

_Bool init = 1;
_Bool play = 0; 

unsigned char *switch_bank_addr = (unsigned char *)0x6000;
unsigned int  *vgm_pos          = (unsigned int *)0x50;
unsigned char *fm1_register     = (unsigned char *)0x4000;
unsigned char *fm1_data         = (unsigned char *)0x4001;
unsigned char *fm2_register     = (unsigned char *)0x4002;
unsigned char *fm2_data         = (unsigned char *)0x4003;
unsigned char *psg_data	        = (unsigned char *)0x7f11;


#define FM1_REGISTER 0x4000
#define FM1_DATA     0x4001
#define FM2_REGISTER 0x4002
#define FM2_DATA     0x4003
#define PSG_DATA     0x7f11


void bank_switch(unsigned char in);
void init_bank(unsigned int addr);
//unsigned int get_offset(unsigned long addr);
void getop();
//void setup_bank(unsigned long addr);
//void play_n_pause();
void stop_sounds();

unsigned char addr_bank;
unsigned char start_bank;
unsigned long start_addr;

union Address curaddr;

//unsigned int curaddr;
unsigned int file_offset;

//unsigned char cur_bank;
// #define _Bool unsigned char
//unsigned char pcm_play = 0x00;
_Bool pcm_play = 0;

unsigned int pcm_datalen;
//unsigned int pcm_addr;
//unsigned int pcm_delay;

union Address pcm_addr;

unsigned char bank_p;
unsigned char pcm_index;
unsigned char vgmbyte;

//unsigned char sfx_play = 0x00;
_Bool sfx_play = 0;
unsigned long sfx_addr = 0x00000000;
//banked_addr sfx_addr;
unsigned int sfx_len = 0x0000;


#define MAX_SAMPLES 16 

/*
unsigned int addrs[MAX_SAMPLES];
unsigned char banks[MAX_SAMPLES];
unsigned long datalens[MAX_SAMPLES];
*/
pcm_record pcms[MAX_SAMPLES];

unsigned int loop_bank;
unsigned int loop_offset;
unsigned char op;
union Address debug_it;
union Address pause_len;
unsigned int initial_pause;   

#define BUFSIZE 0xff
unsigned char vgmbuf1[BUFSIZE];
unsigned char vgmbuf2[BUFSIZE];

unsigned char * vgmbuf; 
unsigned char * fillbuf;

unsigned char pcmbuf[BUFSIZE];

union Address romptr;
unsigned int sample_len;
union Address store_u16;
unsigned long store_u32;
_Bool vgmbanked = 0;
unsigned int pcm_delay;


#define get_offset(x) (x & (~0xFF8000))
#define get_bank(x) (x & (0xFF8000))>>15
#define GET_VGM_POS (curaddr.w - (file_offset+0x8000))

#define WAIT \
 __asm \
  nop \
 __endasm;

#define DELAY \
 __asm \
  nop \
  nop \
  nop \
  nop \
  nop \
  nop \
  nop \
  nop \
  nop \
  nop \
 __endasm;

#define SAMPLEWAIT \
 __asm \
    push ix \
    pop ix \
    push ix \
    pop ix \
    push hl \
    pop hl \
 __endasm;


#define BANK_SWITCH \
__asm \
    ld  hl, (_switch_bank_addr)         \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a             \
    rra                     \
    ld  (hl), a            \
    ld  (hl), l           \
__endasm;

void main(void) {
    
    play = 0;
    init = 0;
    pcm_play = 0;
    pcm_index = 0;
    sfx_play = 0;
    pause_len.w = 0;
    initial_pause = 0;

    for(;;) {
        if (play == 0){ // 30
            stop_sounds();
            DELAY;
        }
        else if(init != 0){ //30
            stop_sounds();
            initialize();
            start();
        }
        else
        {
getbyte:
            //
            // 198 cycles from here to jump to instruction.
            //
            getop();
skipget:
            if(op == 0xe0){
                getop();
                store_u16.l = op;
                getop();
                store_u16.h = op;
                getop();
                getop();

                if(sfx_play == 0) {
                    __asm
                        exx
                        ld de,(_pcms)
                        ld hl,(_store_u16)
                        add hl,de
                        ; hl contains pcm_address

                        ld b,h
                        ld c,l

                        ; Load bank
                        ;inc hl
                        ;ld a,(hl)
                        ;ld (_bank_p),a

                        ld de,(_fm1_data)
                        exx
                    __endasm;
                   
                    bank_p = pcms[0].bank;
                    //pcm_addr.w = pcms[0].addr + store_u16.w;
                    //debug_it.w = pcm_addr.w;
                    /*
                    __asm
                        exx
                        ld de,(_fm1_data)
                        ld hl,#_pcm_addr
                        ld c,(hl)
                        inc hl
                        ld b,(hl)
                        exx
                    __endasm;
                    */
                }
                getop();
            }
            switch(op) { // 121 to jump to position
            case 0x50:
                //op1 = getop();
                getop();
                *psg_data = op;
                break;
            case 0x51:
                getop();
                getop();
                break;
            case 0x52:
                getop();
                *fm1_register = op;
                getop();
                *fm1_data = op;
                break;
            case 0x53:
                getop();
                *fm2_register = op;
                getop();
                *fm2_data = op;
                break;
            case 0x54:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5c:
            case 0x5d:
            case 0x5e:
            case 0x5f:
                break;
            case 0x60:
                break;
            case 0x61:
                //Pause for a set amount of time
                getop();
                pause_len.l = op;
                getop();
                pause_len.h = op;
                initial_pause = pause_len.w;
                //play_n_pause();
                goto play_n_pause;
            case 0x62:
                //Pause for 1/60 of a second 0x02df
                pause_len.w = 0x02df;
                initial_pause = pause_len.w;
                //play_n_pause();
                goto play_n_pause;
            case 0x63:
                //Pause for 1/50 of a second
                pause_len.w = 0x0372;
                initial_pause = pause_len.w;
                //play_n_pause();
                goto play_n_pause;
            case 0x64:
                break;
            case 0x65:
                break;
            case 0x66:
                //Technically song end.  We will wrap around
                start_bank = loop_bank;
                start_addr = loop_offset;
                start();
                break;
            case 0x67:
                //Define a PCM data block
                getop(); //Alway 0x66
                getop(); // tt
                getop();
                //op2 = op; // ss
                store_u16.l = op;
                getop();
                //op3 = op; // ss
                store_u16.h = op;
                getop(); // ss
                getop(); // ss

                //store_u32 |= op4 << 16;
                //store_u32 |= op5 << 24;
               
                __asm
                    ld (_curaddr),bc
                    //exx
                __endasm;

                //addrs[pcm_index] = (curaddr.w);
                //datalens[pcm_index] = store_u16.w;
                //banks[pcm_index] = addr_bank;

                pcms[pcm_index].addr = curaddr.w;
                pcms[pcm_index].bank = addr_bank;
                pcms[pcm_index].datalen = store_u16.w;

                pcm_index+=1;

                //This can overflow!
                if(((curaddr.w-0x8000)+store_u16.w)>=0x8000) {
                    curaddr.w = (curaddr.w - 0x8000) + store_u16.w; 
                    bank_switch(++addr_bank);
                    vgmbanked = 1;
                }
                else{
                    curaddr.w += (store_u16.w) - 4;
                }
                __asm
                    //exx
                    ld bc,(_curaddr)
                __endasm;
                break;
            case 0x68:
            case 0x69:
            case 0x6a:
            case 0x6b:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x6f:
                break;
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
                //Just breaking even here.
                goto getbyte;
            case 0x77:
                SAMPLEWAIT;
                goto getbyte;
            case 0x78:
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x79:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7a:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7b:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7c:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7d:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7e:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x7f:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x80:
                //
                //  Play 1 byte 170 cycles (before pause)
                //      327 worst case

                //pause_len.l = 0x00;
                //goto quickplay;
            case 0x81:
                //pause_len.l = 0x01;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x82:
                //pause_len.l = 0x02;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x83:
                //pause_len.l = 0x03;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x84:
                //pause_len.l = 0x04;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x85:
                //pause_len.l = 0x05;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x86:
                //pause_len.l = 0x06;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x87:
                //pause_len.l = 0x07;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x88:
                //pause_len.l = 0x08;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x89:
                //pause_len.l = 0x09;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8a:
                //pause_len.l = 0x0a;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8b:
                //pause_len.l = 0x0b;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8c:
                //pause_len.l = 0x0c;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8d:
                //pause_len.l = 0x0d;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8e:
                //pause_len.l = 0x0e;
                //initial_pause = pause_len.w;
                //goto quickplay;
            case 0x8f:
                //pause_len.l = 0x0f;
                //initial_pause = pause_len.w;
                //goto quickplay;
quickplay:
                __asm
    ;
    ; de' contains (_fm1_data)
    ; bc' contains #_pcm_addr
    ;

    ;
    ; Get the shadow registers
    ;    
    exx

;vgm_drv.c:525: if(pcm_addr.h == 0x00) {
    ;
    ; Check for overflow
    ;
	ld	a,b
	or	a, a
	jr	NZ,00572$
;vgm_drv.c:526: pcm_addr.h = 0x80;
	ld	b,#0x80
;vgm_drv.c:527: bank_p++;  
	ld	iy,#_bank_p
	inc	0 (iy)

;vgm_drv.c:528: bank_switch(bank_p); 
    ;
    ; Bank switch!
    ;
	ld	a,(_bank_p)
    ld  hl, (_switch_bank_addr)         ; hl = bankreg  
    ld  (hl), a             ; #1 (bit 15)              
    rra                     ;                         
    ld  (hl), a             ; #2 (bit 16)            
    rra                     ;                       
    ld  (hl), a             ; #3 (bit 17)              
    rra                     ;                         
    ld  (hl), a             ; #4 (bit 18)            
    rra                     ;                       
    ld  (hl), a             ; #5 (bit 19)          
    rra                     ;                     
    ld  (hl), a             ; #6 (bit 20)        
    rra                     ;                           
    ld  (hl), a             ; #7 (bit 21)              
    rra                     ;                         
    ld  (hl), a             ; #8 (bit 22)            
    ld  (hl), l             ; #9 (bit 23 = 0)       
;vgm_drv.c:529: vgmbanked = 0;
	ld	iy,#_vgmbanked
	ld	0 (iy),#0x00
	jr	00573$

00572$:
;vgm_drv.c:530: } else if (vgmbanked == 1) {
    ;
    ; Check if vgm processing changed the bank
    ;
	ld	iy,#_vgmbanked
	ld	a,0 (iy)
	sub	a, #0x01
	jr	NZ,00573$

;vgm_drv.c:531: bank_switch(bank_p); 
    ;
    ;  Switch banks!
    ;
	ld	a,(_bank_p)
    ld  hl, (_switch_bank_addr)         ; hl = bankreg  
    ld  (hl), a             ; #1 (bit 15)              
    rra                     ;                         
    ld  (hl), a             ; #2 (bit 16)            
    rra                     ;                       
    ld  (hl), a             ; #3 (bit 17)              
    rra                     ;                         
    ld  (hl), a             ; #4 (bit 18)            
    rra                     ;                       
    ld  (hl), a             ; #5 (bit 19)          
    rra                     ;                     
    ld  (hl), a             ; #6 (bit 20)        
    rra                     ;                           
    ld  (hl), a             ; #7 (bit 21)              
    rra                     ;                         
    ld  (hl), a             ; #8 (bit 22)            
    ld  (hl), l             ; #9 (bit 23 = 0)       
;vgm_drv.c:532: vgmbanked = 0;
	ld	iy,#_vgmbanked
	ld	0 (iy),#0x00

00573$:
;vgm_drv.c:546:
    ;
    ;  Setup the fm channel for pcm play
    ; 
;	*fm1_register = 0x2a
	ld	hl,#0x4000
	ld	(hl),#0x2A
;	*fm1_data = *(unsigned char)pcm_addr.w
    ;
    ;  Play a byte!
    ;    
	ld	a,(bc)
	ld	(de),a

;vgm_drv.c:550: pcm_addr.l++;
    ;
    ;  Increment the pcm index
    ;
	inc	c
;vgm_drv.c:551: if(pcm_addr.l == 0) {
	jp	NZ,00574$
;vgm_drv.c:552: pcm_addr.h++;
	inc	b
;vgm_drv.c:554: goto getbyte;
00574$:

    ;
    ;  Switch the registers back
    ;
    exx

    ;
    ;  Go to getbyte
    ;     
    ;jp 00101$
    __endasm;
            getop();
            if((op & 0xf0) == 0x80) {
                goto quickplay;
            } else {
                goto skipget;
            }
            goto pause;

            case 0x90:
                //Setup Stream Control
                getop();
                getop();
                //op1 = getop(); //ss
                //op2 = getop(); //tt
                //op3 = getop(); //pp
                //op4 = getop(); //cc
                getop();
                *fm1_register = op;
                getop();
                *fm1_data = op;
                break;
            case 0x91:
                //Set Stream Data
                getop();
                getop();
                getop();
                getop();
                /*
                op1 = getop(); //ss
                op2 = getop(); //dd
                op3 = getop(); //ll
                op4 = getop(); //bb
                */
                break;
            case 0x92:
                //Set Stream Freq
                /*
                op1 = getop();
                op2 = getop(); //ff
                op3 = getop(); //ff
                op4 = getop(); //ff
                op5 = getop(); //ff 
                */
                getop();
                getop();
                //store_u16.l = op;
                getop();
                //store_u16.h = op;
                getop();
                getop();
                //store_u16 |= op3 << 8;
                //store_u32 |= op4 << 16;
                //store_u32 |= op5 << 24;

                /*
                switch (store_u16.w)
                {
                    case 0x1f40:
                        pcm_delay = 3;
                        break;
                    case 0x2b11:
                        pcm_delay = 1;
                        break;
                    default:
                        pcm_delay = 3;
                }
                */
                break;
            case 0x93:
                getop();
                getop();
                getop();
                getop();
                getop();
                getop();
                getop();
                getop();
                getop();
                getop();
                break;
            case 0x94:
                getop();
                //op1 = getop();
                pcm_play = 0;
                break;
            case 0x95:
                //Play stream
                getop();
                getop();
                pcm_index = op;
                getop();
                getop();
                
                pcm_play = 1;
                if(sfx_play == 0) {
                    //pcm_addr.w = pcms[pcm_index].addr;
                    //bank_p = pcms[pcm_index].bank;
                    //pcm_datalen = pcms[pcm_index].datalen;

                    //debug_it.w = (unsigned int)&pcms;

                    __asm
                        exx
                        ld de,#_pcms
                        ld hl,(_pcm_index)
                        add hl,hl
                        add hl,hl
                        add hl,hl
                        add hl,de
                        ; hl contains address of record

                        ; Load pcm_addr
                        ld c,(hl)
                        inc hl
                        ld b,(hl)
                        ;ld (_pcm_addr),bc
                        
                        ; Load bank
                        inc hl
                        ld a,(hl)
                        ld (_bank_p),a

                        ; Load datalen
                        inc hl
                        inc hl
                        ld e,(hl)
                        inc hl
                        ld d,(hl)
                        ld (_pcm_datalen),de

                        ld de,(_fm1_data)
                        exx
                    __endasm;
                }

                goto getbyte;
            }
            if(sfx_play == 1 && pcm_play == 0) {
                __asm 
                    exx
                __endasm;
                bank_p = (sfx_addr & (0xFF8000))>>15;
                pcm_addr.w = (sfx_addr & ~0xff8000) + 0x8000;
                pcm_datalen = sfx_len;
                //debug_it.w = (sfx_addr & ~0xff8000) + 0x8000;
                pcm_play = 1;
                __asm
                    ;exx
                    ld de,(_fm1_data)
                    ld hl,#_pcm_addr
                    ld c,(hl)
                    inc hl
                    ld b,(hl)
                    exx
                __endasm;
            }
        }
    }
play_n_pause:
                __asm

    ;
    ; de' contains (_fm1_data)
    ; bc' contains #_pcm_addr
    ;

;vgm_drv.c:824: if(pcm_play == 1){
;
;   Only play if pcm_play == 1
;
    ld a,(_pcm_play)
    sub a,#0x01
	;ld	iy,#_pcm_play
	;ld	a,0 (iy)
	;sub	a, #0x01
	jp	NZ,00612$

;vgm_drv.c:832: if (vgmbanked == 1) {
;
;  Check the bank
;
	ld	a,(#_vgmbanked + 0)
	sub	a, #0x01
	jr	NZ,00587$
;vgm_drv.c:833: bank_switch(bank_p); 
	ld	a,(_bank_p)
    ld  hl, (_switch_bank_addr)         ; hl = bankreg  
    ld  (hl), a             ; #1 (bit 15)              
    rra                     ;                         
    ld  (hl), a             ; #2 (bit 16)            
    rra                     ;                       
    ld  (hl), a             ; #3 (bit 17)              
    rra                     ;                         
    ld  (hl), a             ; #4 (bit 18)            
    rra                     ;                       
    ld  (hl), a             ; #5 (bit 19)          
    rra                     ;                     
    ld  (hl), a             ; #6 (bit 20)        
    rra                     ;                           
    ld  (hl), a             ; #7 (bit 21)              
    rra                     ;                         
    ld  (hl), a             ; #8 (bit 22)            
    ld  (hl), l             ; #9 (bit 23 = 0)       
;vgm_drv.c:834: vgmbanked = 0;
    xor a,a
    ld (_vgmbanked), a
	;ld	iy,#_vgmbanked
	;ld	0 (iy),#0x00
;
;  Banking chews up time
;
    ld	hl,(_pause_len)
	dec	hl
	dec	hl
	ld	(_pause_len),hl

00587$:
;vgm_drv.c:843: *fm1_register = 0x2A; // 34
;
;   Setup the register for PCM play
;
	ld	hl,(_fm1_register)
	ld	(hl),#0x2A

00609$:

;
;   THIS IS INCORRECT
;

;vgm_drv.c:848: if(pause_len.w == 0)
;
;   Check the remaining pause length
;
    ld de,(_pause_len)
    ld a,(_initial_pause)
    sub a,e
    ld a,(_initial_pause + 0x0001)
    sbc a,d
    jp C,00101$

;vgm_drv.c:855: if(pcm_datalen == 0) { // 
;
;   Check the remaining PCM bytes
;
	;ld	iy,#_pcm_datalen
	;ld	a,1 (iy)
	;or	a,0 (iy)
    ld hl,#_pcm_datalen
    ld d,(hl)
    inc hl
    ld a,(hl)
    or a,d
	jr	NZ,00591$

;vgm_drv.c:856: pcm_play = 0;//
;
;   Done with PCM, unset pcm_play and sfx_play
;
    xor a,a
    ld (_pcm_play), a
	;ld	hl,#_pcm_play + 0
	;ld	(hl), #0x00
;vgm_drv.c:857: sfx_play = 0;// ??
    ld (_sfx_play),a
    ;ld hl,#_sfx_play
    ;ld (hl), #0x00
	;ld	iy,#_sfx_play
	;ld	0 (iy),#0x00
;vgm_drv.c:858: goto pause;
	jr	00612$

00591$:
;vgm_drv.c:860: pcm_datalen--;  //
	ld	hl,(_pcm_datalen)
	dec	hl
	ld	(_pcm_datalen),hl

    exx
;vgm_drv.c:862: if(pcm_addr.h == 0x00) {
;
;   Check to see if we need to bank
;
    ld a,b
	or	a, a
	jr	NZ,00593$
;vgm_drv.c:863: pcm_addr.h = 0x80;
	;ld	(hl),#0x80
    ld b,#0x80
;vgm_drv.c:864: bank_p++;  
;
;   Increase the bank pointer
;
	;ld	iy,#_bank_p
	;inc	0 (iy)
    ld a,(_bank_p)
    inc a
    ld (_bank_p),a
;vgm_drv.c:865: bank_switch(bank_p); 
	;ld	a,(_bank_p)
    ld  hl, (_switch_bank_addr)         ; hl = bankreg  
    ld  (hl), a             ; #1 (bit 15)              
    rra                     ;                         
    ld  (hl), a             ; #2 (bit 16)            
    rra                     ;                       
    ld  (hl), a             ; #3 (bit 17)              
    rra                     ;                         
    ld  (hl), a             ; #4 (bit 18)            
    rra                     ;                       
    ld  (hl), a             ; #5 (bit 19)          
    rra                     ;                     
    ld  (hl), a             ; #6 (bit 20)        
    rra                     ;                           
    ld  (hl), a             ; #7 (bit 21)              
    rra                     ;                         
    ld  (hl), a             ; #8 (bit 22)            
    ld  (hl), l             ; #9 (bit 23 = 0)       
;vgm_drv.c:866: vgmbanked = 0;
	;ld	iy,#_vgmbanked
	;ld	0 (iy),#0x00
    xor a,a
    ld (_vgmbanked), a
;
;  Banking chews up time
;
    ld	hl,(_pause_len)
	dec	hl
	dec	hl
	ld	(_pause_len),hl

00593$:
;vgm_drv.c:875: *fm1_data = *(unsigned char*)pcm_addr.w;
;
;   Play a PCM byte!
;
	ld	a,(bc)
	ld	(de),a

    push ix
    pop ix
    push ix
    pop ix
    push ix
    pop ix
    push ix
    pop ix
    push ix
    pop ix

;vgm_drv.c:877: pcm_addr.l++;
	inc	c
;vgm_drv.c:878: if(pcm_addr.l == 0) {
	jr	NZ,00595$
;vgm_drv.c:879: pcm_addr.h++;
    inc b
00595$:
    exx 	
    ld	hl,(_pause_len)
	dec	hl
	dec	hl
	dec	hl
	dec	hl
	dec	hl
	ld	(_pause_len),hl
	jp	00609$
00612$:
    __endasm;
pause:
#if 0
    __asm
00612$:
    ld hl,(_pause_len)
    ld de,(_initial_pause)
00613$:
    ex de,hl
    ld a,l
    sub a,d
    ld a,h
    sbc a,e
    l
#endif

    __asm
        ld	hl,(_pause_len)
        dec	hl
        dec	hl
        dec	hl
        ld	(_pause_len),hl
    __endasm;    

    for(;;) {
        if(initial_pause < pause_len.w) {
            goto getbyte;
        }
        __asm
            ld	hl,(_pause_len)
            dec	hl
            dec	hl
            ld	(_pause_len),hl
        __endasm;    
        //pause_len.w--;
        //DELAY;
    }
    goto getbyte;

    __asm
;00612$:
    ld hl,(_pause_len)
00613$:
    ld d,h
    ld a,l
    or a,d
	jp	Z,00614$
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    dec hl
    jp 00613$
00614$:
    ld hl,#_pause_len
    xor a,a
    ld (hl),a
    inc hl
    ld (hl),a
    jp 00101$
    __endasm;
#if 0
    __asm
00612$:
    ld bc,#_pause_len
    ;ld hl,#_pause_len
    ;ld b,(hl)
    ;inc hl
    ;ld c,(hl)
00613$:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    djnz 00613$
    ld hl,#_pause_len
    xor a,a
    ld (hl),a
    inc hl
    ld (hl),a
    jp 00101$

;vgm_drv.c:982: if(pause_len.w == 0){
	;ld	hl,#_pause_len
	;ld	d,(hl)
	;inc	hl
	;ld	e,(hl)
	;ld	a,e
	;or	a,d
	;jp	Z,00101$
	
    ;ld	hl,(_pause_len)
	;dec	hl
	;ld	(_pause_len),hl
;vgm_drv.c:995: }
	;jr	00613$
                __endasm;
#endif

}


void start(void){
    int buf_offset;

    curaddr.w = start_addr + 0x8000;

    //debug_it.w = GET_VGM_POS;

    addr_bank = start_bank;
    pcm_play = 0; 
    pcm_index = 0;
    //setup_bank(start_addr);
    bank_switch(addr_bank);
    vgmbanked = 1; 
    //fill_buf();
    //swap_bufs();
    
    debug_it.w = 0xffff;
    
    __asm
        ld bc,(_curaddr)
    __endasm;
}
void initialize(void){
    unsigned long *vgm_data_offset;
    unsigned long loop_addr;

    //Find the offset within a bank for the VGM file
    file_offset = get_offset(addr);
    //Find the starting bank for the VGM file
    start_bank = get_bank(addr);
    //start_bank = (addr & (0xFF8000)) >> 15;
    //Switch to the starting bank
    bank_switch(start_bank);
    //cur_bank = start_bank;
  
    if((0x0034 + file_offset)>=0x8000) {
        // We wrapped around, go to the new bank
        file_offset = (file_offset + 0x0034) - 0x8000;
        bank_switch(++start_bank);
    }
    //Get the VGM data location
    vgm_data_offset = (unsigned long *) (0x0034+file_offset+0x8000);
    
    //Calculate the vgm_data starting location (really need to check the bank
    //here.
    // Note that our start_offset can wrap around.
    start_addr = *vgm_data_offset + 0x34 + file_offset;   

    loop_addr = ( *((unsigned long *) (0x001C + file_offset + 0x8000)) + 0x1C + addr);
    loop_bank = (loop_addr & (0xFF8000))>>15;
    loop_offset = get_offset(loop_addr);

    vgmbanked = 1;
    init = 0;
}

void bank_switch(unsigned char in) {
    __asm
        ld  a,4 (ix)
        ld  hl, (_switch_bank_addr)         ; hl = bankreg  
        ld  (hl), a             ; #1 (bit 15)              
        rra                     ;                         
        ld  (hl), a             ; #2 (bit 16)            
        rra                     ;                       
        ld  (hl), a             ; #3 (bit 17)              
        rra                     ;                         
        ld  (hl), a             ; #4 (bit 18)            
        rra                     ;                       
        ld  (hl), a             ; #5 (bit 19)          
        rra                     ;                     
        ld  (hl), a             ; #6 (bit 20)        
        rra                     ;                           
        ld  (hl), a             ; #7 (bit 21)              
        rra                     ;                         
        ld  (hl), a             ; #8 (bit 22)            
        ld  (hl), l             ; #9 (bit 23 = 0)       
    __endasm;
}

void getop()
{

    // Best case 60 cycles
    // One bank 189 cycles
    // Worst case (two banks) 386 cycles

    __asm
;vgm_drv.c:1220: if(vgmbanked == 0) {
	ld	hl,#_vgmbanked+0
	bit	0, (hl)
	jr	NZ,00502$
;vgm_drv.c:1222: vgmbanked = 1;
	ld	(hl), #0x01
;vgm_drv.c:1221: bank_switch(addr_bank);
	ld	a,(_addr_bank)
	ld	hl, (_switch_bank_addr) ; hl = bankreg
	ld	(hl), a ; #1 (bit 15)
	rra	;
	ld	(hl), a ; #2 (bit 16)
	rra	;
	ld	(hl), a ; #3 (bit 17)
	rra	;
	ld	(hl), a ; #4 (bit 18)
	rra	;
	ld	(hl), a ; #5 (bit 19)
	rra	;
	ld	(hl), a ; #6 (bit 20)
	rra	;
	ld	(hl), a ; #7 (bit 21)
	rra	;
	ld	(hl), a ; #8 (bit 22)
	ld	(hl), l ; #9 (bit 23 = 0)
00502$:
;vgm_drv.c:1225: op = *(unsigned char *)curaddr.w;
    ;ld bc,(_curaddr)
    ld a,(bc)
    ld d,a
	;ld	(#_op + 0),a
;vgm_drv.c:1226: curaddr.l++;
	inc	c
;vgm_drv.c:1227: if(curaddr.l == 0) { 
    ;JP NZ,00503$
    ret nz
;vgm_drv.c:1228: curaddr.h++;
	inc	b
;vgm_drv.c:1229: if(curaddr.h == 0x00) {
    ;JP NZ,00503$
    ret nz
;vgm_drv.c:1230: curaddr.h = 0x80;
    ld b,#0x80
;vgm_drv.c:1231: addr_bank++;
	ld	iy,#_addr_bank
	inc	0 (iy)
;vgm_drv.c:1232: bank_switch(addr_bank);
	ld	a,(_addr_bank)
	ld	hl, (_switch_bank_addr) ; hl = bankreg
	ld	(hl), a ; #1 (bit 15)
	rra	;
	ld	(hl), a ; #2 (bit 16)
	rra	;
	ld	(hl), a ; #3 (bit 17)
	rra	;
	ld	(hl), a ; #4 (bit 18)
	rra	;
	ld	(hl), a ; #5 (bit 19)
	rra	;
	ld	(hl), a ; #6 (bit 20)
	rra	;
	ld	(hl), a ; #7 (bit 21)
	rra	;
	ld	(hl), a ; #8 (bit 22)
	ld	(hl), l ; #9 (bit 23 = 0)
;vgm_drv.c:1233: vgmbanked = 1;
	ld	hl,#_vgmbanked + 0
	ld	(hl), #0x01
00503$:
    ;ld (_curaddr),bc
	ret
    __endasm;

//
#if 0
    if(vgmbanked == 0) {
        bank_switch(addr_bank);
        vgmbanked = 1;
    }

    op = *(unsigned char *)curaddr.w;
    curaddr.l++;
    if(curaddr.l == 0) { 
        curaddr.h++;
        if(curaddr.h == 0x00) {
            curaddr.h = 0x80;
            addr_bank++;
            bank_switch(addr_bank);
            vgmbanked = 1;
        } 
    }
#endif
}

void stop_sounds()
{
    *fm1_register = 0x28;
    *fm1_data = 0x00;
    *fm1_data = 0x04;
    *fm1_data = 0x01;
    *fm1_data = 0x05;
    *fm1_data = 0x02;
    *fm1_data = 0x06;

    *psg_data = 0x80;
    *psg_data = 0x00;
    *psg_data = 0x9f; 

    *psg_data = 0xa0;
    *psg_data = 0x00;
    *psg_data = 0xbf; 

    *psg_data = 0xc0;
    *psg_data = 0x00;
    *psg_data = 0xdf; 

    *psg_data = 0xe0;
    *psg_data = 0x00;
    *psg_data = 0xff; 
}
