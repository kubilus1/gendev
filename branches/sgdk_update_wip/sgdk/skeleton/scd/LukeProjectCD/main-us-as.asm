; =======================================================================================
;  Sega CD Boot loader by Luke Usher/SoullessSentinel (c) 2010
;  Used with permission. This code is not released under the GPL.
; =======================================================================================
;align macro
;	cnop 0,\1
;	endm
; =======================================================================================
;  Sega CD Header (Based on Sonic CD's header)
; =======================================================================================
DiscType:	dc.b 'SEGADISCSYSTEM  '		; Disc Type (Must be SEGADISCSYSTEM)
VolumeName:	dc.b 'SEGACDGAME ',0		; Disc ID
VolumeSystem:	dc.w $100,$1			; System ID, Type
SystemName:	dc.b 'SEGACDGAME ',0		; System Name
SystemVersion:	dc.w 0,0			; System Version, Type
IP_Addr:	dc.l $800			; IP Start Address (Must be $800)
IP_Size:	dc.l $800			; IP End Address (Must be $800)
IP_Entry:	dc.l 0
IP_WorkRAM:	dc.l 0
SP_Addr:	dc.l $1000			; SP Start Address (Must be $1000)
SP_Size:	dc.l $7000			; SP End Address (Must be $7000)
SP_Entry:	dc.l 0
SP_WorkRAM:	dc.l 0
        .mri 0
		.org 0x00000100
        .mri 1
		
; =======================================================================================
;  Game Header
; =======================================================================================	
HardwareType:	dc.b 'SEGA MEGA DRIVE '
Copyright:	dc.b '(C)MKUBILUS 2013'
NativeName:	dc.b 'Space test                                      '
OverseasName:	dc.b 'Space test                                      '
DiscID:		dc.b 'GM 00-0000-00   '
IO:		dc.b 'J               '
		; Modem information, notes, and padding, left undefined as it is not used
		; Padded to $1F0 instead (Start of Region Code)
        .mri 0
		.org 0x000001F0
        .mri 1
Region:		dc.b 'E               '
; ========================================================================================
;  IP (Includes security sector)
; ========================================================================================	
		;incbin "_boot/ip-us.bin";
		include _boot/ip-us-as.asm
; =======================================================================================
;  Sub CPU Program (SP)
; =======================================================================================
        .mri 0
        .org 0x00001000
        .mri 1
		;incbin	"_boot/sp.bin"
		;incbin	"_boot/sp-as.bin"
        include _boot/sp-as.asm
; =======================================================================================
;  Padding, to $8000 (Size of boot area of iso)
; =======================================================================================	
        .mri 0
        .org 0x00008000
        .mri 1
; =======================================================================================
;  FileSystem:
; =======================================================================================
		incbin	"filesystem.bin"
        .mri 0
        .org 0x02000000
        .mri 1
