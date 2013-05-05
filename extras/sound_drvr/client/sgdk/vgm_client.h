#ifndef Z80VGM
#define Z80VGM
void VGM_load_driver(void);
void VGM_load_song(unsigned int addr);
void VGM_start(void);
void VGM_stop(void);
#endif
