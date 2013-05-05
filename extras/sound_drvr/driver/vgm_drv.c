/* first edit sdcc source-code src/device/lib/z80/crt0.s and change the 
 * stack pointer to 0x1fff
 *
 */

unsigned long addr;

unsigned char init = 0x01;
unsigned char play = 0x00; 

unsigned char *switch_bank_addr = (unsigned char *)0x6000;
unsigned int  *vgm_pos          = (unsigned int *)0x50;
unsigned char *fm1_register     = (unsigned char *)0x4000;
unsigned char *fm1_data         = (unsigned char *)0x4001;
unsigned char *fm2_register     = (unsigned char *)0x4002;
unsigned char *fm2_data         = (unsigned char *)0x4003;
unsigned char *psg_data	        = (unsigned char *)0x7f11;

void bank_switch(unsigned char in);
void init_bank(unsigned int addr);
unsigned int get_offset(unsigned long addr);
unsigned char getc();
void setup_bank(unsigned long addr);
void play_n_pause(unsigned int pause);
void stop_sounds();

unsigned char addr_bank;
unsigned char start_bank;
unsigned long start_addr;
unsigned int curaddr;
unsigned int file_offset;

unsigned char cur_bank;

unsigned char pcm_play = 0x00;

unsigned int pcm_datalen;
unsigned int pcm_addr;
//unsigned int pcm_delay;

unsigned int bank_p;
unsigned char pcm_index;

#define MAX_SAMPLES 16 

unsigned int addrs[MAX_SAMPLES];
unsigned char banks[MAX_SAMPLES];
unsigned long datalens[MAX_SAMPLES];

unsigned int loop_bank;
unsigned int loop_offset;

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



//void nop(void) {
//}

void main(void) {
    unsigned int i;
    unsigned int j;
    unsigned int pause_len=0;
    unsigned long store_u32;
    unsigned int store_u16;
    unsigned char opcode, op1, op2, op3, op4, op5;
    unsigned char *pb;

    play = 0x00;
    init = 0x00;
    pcm_play = 0x00;
    pcm_index = 0;

    for(;;) {
        if (play == 0x00){ // 30
            stop_sounds();
            DELAY;
        }
        else if(init != 0x00){ //30
            stop_sounds();
            initialize();
            start();
        }
        else
        {
getbyte:
            opcode = getc(); // 248
skipget:
            switch(opcode) { // 125 to jump to position
            case 0x50:
                op1 = getc();
                *psg_data = op1;
                break;
            case 0x51:
                break;
            case 0x52:
                op1 = getc();
                op2 = getc();
                *fm1_register = op1;
                *fm1_data = op2;
                break;
            case 0x53:
                op1 = getc();
                op2 = getc();
                *fm2_register = op1;
                *fm2_data = op2;
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
                op1 = getc();
                op2 = getc();
                pause_len = op1;
                pause_len|= op2<<8;
                play_n_pause(pause_len);
                break;
            case 0x62:
                //Pause for 1/60 of a second 0x02df
                //play_n_pause(0x02df);
                //break;
                pause_len=0; // 38
                for(;;)
                {
                    if(opcode != 0x62) { //if true: 32  else 18
                        play_n_pause(pause_len); // 77
                        // 20
                        break;
                    }
                    if(pause_len > 0xfd20) { // if true 59, else 64
                        play_n_pause(pause_len); // 65
                        // 11
                        pause_len = 0; // 38
                    };
                    // Reduce the pause by 5
                    pause_len+=0x02da; // 81
                    opcode = getc(); // 244
                    // 37
                }
                goto skipget;
                break;
            case 0x63:
                //Pause for 1/50 of a second
                play_n_pause(0x0372);
                break;
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
                getc(); //Alway 0x66
                op1 = getc(); // tt
                op2 = getc(); // ss
                op3 = getc(); // ss
                op4 = getc(); // ss
                op5 = getc(); // ss

                store_u16 = op2;
                store_u16 |= op3 << 8;
                //store_u32 |= op4 << 16;
                //store_u32 |= op5 << 24;
                
                addrs[pcm_index] = (curaddr + 0x8000);
                datalens[pcm_index] = store_u16;
                banks[pcm_index] = addr_bank;

                pcm_index+=1;

                //This can overflow!
                if((curaddr+store_u16)>=0x8000) {
                    curaddr = (curaddr+store_u16) - 0x8000; 
                    bank_switch(++addr_bank);
                }
                else{
                    curaddr += (store_u16) - 4;
                }
                
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
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
                goto getbyte;
            case 0x87:
                SAMPLEWAIT;
                goto getbyte;
            case 0x88:
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x89:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8a:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8b:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8c:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8d:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8e:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                goto getbyte;
            case 0x8f:
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                SAMPLEWAIT;
                // 0x8n, play n number of PCM bytes.
                //play_n_pause((opcode & 0x0f));
                goto getbyte;
            case 0x90:
                //Setup Stream Control
                op1 = getc(); //ss
                op2 = getc(); //tt
                op3 = getc(); //pp
                op4 = getc(); //cc
                *fm1_register = op4;
                *fm1_data = op3;
                break;
            case 0x91:
                //Set Stream Data
                op1 = getc(); //ss
                op2 = getc(); //dd
                op3 = getc(); //ll
                op4 = getc(); //bb
                break;
            case 0x92:
                //Set Stream Freq
                op1 = getc();
                op2 = getc(); //ff
                op3 = getc(); //ff
                op4 = getc(); //ff
                op5 = getc(); //ff 
                //store_u16 = op2;
                //store_u16 |= op3 << 8;
                //store_u32 |= op4 << 16;
                //store_u32 |= op5 << 24;

                /*
                switch (store_u16)
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
                break;
            case 0x94:
                op1 = getc();
                pcm_play = 0x00;
                break;
            case 0x95:
                //Play stream
                op1 = getc(); //ss
                op2 = getc(); //bb
                op3 = getc(); //ff
                pcm_play = 0x01;
                pcm_index = op2;
                bank_p = banks[pcm_index];
                pcm_datalen = datalens[pcm_index];
                pcm_addr = addrs[pcm_index];
                break;
            /*    
            case 0xe0:
                // Seek to PCM offset.  Not implemented yet.
                op1 = getc();
                op2 = getc();
                op3 = getc();
                op4 = getc();
                break;
            */
            }
        }
    }
}

void play_n_pause(unsigned int pause){
    /*
     * play_n_pause (int pause)
     *
     *   pause -> How many samples to wait for
     *
     *   One sample =~ 81 z80 cycles
     *
     *   Overhead: roughly 337 cycles without pcm
     *
     */

    // 72 cycles

    unsigned char switched = 0x00; // 19 cycles
    unsigned int initial_pause;  // 
    initial_pause = pause;  // 76

    // Reduce pause due to overhead.
    pause = pause - 10;  // 89 ...

    if(pcm_play == 0x01){
        // 41

        *fm1_register = 0x2A; // 34

        if (bank_p != addr_bank){ // 98
            pause -= 8;    
            bank_switch(bank_p); // 503
            switched = 0x01;
        }

        for(;;)
        { 
            //
            // Base cycle time of inner loop: ~ 498 cycles
            //
            // Close to 8000Hz sample rate for PCM.

            if(pcm_datalen == 0) { // 58
                pcm_play = 0x00;//43
                break;
            }
            if(pause > initial_pause) break; // 47
            pcm_datalen--;  //42
            pause -= 5; // 68
            *fm1_data = *((unsigned char *)(pcm_addr++)); // 207 

            if(pcm_addr == 0xFFFF) { // 76
                pause -= 8;
                pcm_addr = 0x8000;
                bank_switch(++bank_p); //168 or 503
                switched = 0x01;
            }

        }
        if(switched == 0x01){
            pause -= 7;
            bank_switch(addr_bank); // 503
        }
    }
    // 10
    for(;pause<=initial_pause;pause--){  // 71
        __asm 
            ld de,#0x0000 ;; Delay of 10 
        __endasm;
        // 81 cycle wait total.
    }
}

void start(void){
    curaddr = start_addr;
    addr_bank = start_bank;
    pcm_play = 0x00; 
    pcm_index = 0;
    //setup_bank(start_addr);
    bank_switch(addr_bank);
}

void initialize(void){
    unsigned long *vgm_data_offset;
    unsigned long loop_addr;

    //Find the offset within a bank for the VGM file
    file_offset = get_offset(addr);
    //Find the starting bank for the VGM file
    start_bank = (addr & (0xFF8000))>>15;
    //Switch to the starting bank
    bank_switch(start_bank);
    cur_bank = start_bank;
  
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

    init = 0x00;
}


void setup_bank(unsigned long addr){
    unsigned char *pb;
    pb = (unsigned char *)0x6000;
    pb[0x00] = addr >> 15;
    pb[0x00] = addr >> 16;
    pb[0x00] = addr >> 17;
    pb[0x00] = addr >> 18;
    pb[0x00] = addr >> 19;
    pb[0x00] = addr >> 20;
    pb[0x00] = addr >> 21;
    pb[0x00] = addr >> 22;
    pb[0x00] = addr >> 23;
}

unsigned int get_offset(unsigned long addr)
{
    return (unsigned int) (addr & (~0xFF8000));
}


void bank_switch(unsigned char in) {
    /*
     * Overhead.  If switching 503 cycles,
     * If not bank switching: 168 cycles
     *
     */

    //62

    if (cur_bank != in) // If true 59, else 64
    {    
        __asm
        ;vgm_drv.c:353: *switch_bank_addr = (in)&1;
            ld	bc,(_switch_bank_addr)      ;20
            ld	a,4 (ix)                    ;19
            ld  h,a                         ;4
            and	a,#0x01                     ;7 
            ld	(bc),a                      ;7   = 57
        ;vgm_drv.c:354: *switch_bank_addr = (in>>1)&1;
            ld  a,h                         ;4
            srl	a                           ;8
            and	a,#0x01                     ;7
            ld	(bc),a                      ;7   = 26
        ;vgm_drv.c:355: *switch_bank_addr = (in>>2)&1;
            ld a,h 
            srl	a
            srl	a
            and	a,#0x01
            ld	(bc),a                      ; = 34
        ;vgm_drv.c:356: *switch_bank_addr = (in>>3)&1;
            ld a,h
            srl	a
            srl	a
            srl	a
            and	a,#0x01
            ld	(bc),a                      ; = 42
        ;vgm_drv.c:357: *switch_bank_addr = (in>>4)&1;
            ld  a,h
            srl	a
            srl	a
            srl	a
            srl	a
            and	a,#0x01
            ld	(bc),a                      ; = 50
        ;vgm_drv.c:358: *switch_bank_addr = (in>>5)&1;
            ld  a,h
            srl	a
            srl	a
            srl	a
            srl	a
            srl	a
            and	a,#0x01
            ld	(bc),a                      ; = 58
        ;vgm_drv.c:359: *switch_bank_addr = (in>>6)&1;
            ld  e,h    
            srl	e
            srl	e
            srl	e
            srl	e
            srl	e
            srl	e
            ld	a,e
            and	a,#0x01
            ld	(bc),a                      ; = 66
        ;vgm_drv.c:360: *switch_bank_addr = (in>>7)&1;
            ld  a,h
            rlc	a
            and	a,#0x01
            ld	(bc),a                      ; = 26 
        ;vgm_drv.c:361: *switch_bank_addr = (in>>8)&1;
            ld	a,#0x00
            ld	(bc),a                      ; = 14    
        ;vgm_drv.c:363: cur_bank = in;
            ld  a,h
            ld	hl,#_cur_bank + 0
            ld	(hl), a                     ; = 24
        __endasm;
       // 340 cycles 
    }  

    //42 
}

unsigned char getc() {
    /*
     *  Cycles: ~244
     */

    // 18
    if(curaddr==0x8000) { // If true ~61 else ~56
        curaddr = (curaddr) - 0x8000; 
        bank_switch(++addr_bank);
    }

    return *((unsigned char *)(curaddr++) + 0x8000); //160
    //10
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
