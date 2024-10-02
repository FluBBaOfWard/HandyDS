#ifdef __arm__

//#define EMBEDDED_ROM

#include "ARMSuzy/ARMSuzy.i"

	.global machineInit
	.global loadCart
	.global romNum
	.global cartFlags
	.global romStart

	.global romSpacePtr
	.global MEMMAPTBL_

	.global lynxRAM
	.global gRomSize
	.global maxRomSize
	.global romMask
	.global gConfig
	.global gMachine
	.global gMachineSet
	.global gSOC
	.global gLang
	.global gPaletteBank

	.syntax unified
	.arm

	.section .rodata
	.align 2

#ifdef EMBEDDED_ROM
ROM_Space:
//	.incbin "roms/APB - All Points Bulletin (1990).lnx"
//	.incbin "roms/Batman Returns (1992).lnx"
//	.incbin "roms/Double Dragon (1993) (Telegames).lnx"
//	.incbin "roms/Dracula - The Undead (1991).lnx"
ROM_SpaceEnd:
#endif
LYNX_BIOS_INTERNAL:
	.incbin "roms/lynxboot.img"

	.align 2
;@----------------------------------------------------------------------------
machineInit: 				;@ Called from C
	.type   machineInit STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r11,lr}

#ifdef EMBEDDED_ROM
	ldr r0,=romSize
	mov r1,#ROM_SpaceEnd-ROM_Space
	str r1,[r0]
	ldr r0,=romSpacePtr
	ldr r7,=ROM_Space
	str r7,[r0]
#endif
	bl gfxInit
//	bl ioInit
	bl soundInit

	ldmfd sp!,{r4-r11,lr}
	bx lr

	.section .ewram,"ax"
	.align 2
;@----------------------------------------------------------------------------
loadCart: 					;@ Called from C
	.type   loadCart STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r11,lr}

	ldr r0,romSize
	movs r1,r0,lsr#14			;@ 16kB blocks.
	subne r1,r1,#1
	str r1,romMask				;@ romMask=romBlocks-1

	ldrb r5,gMachine
	cmp r5,#HW_LYNX_II
	moveq r4,#SOC_HOWARD2
	movne r4,#SOC_HOWARD
	strb r4,gSOC

	ldr r0,=lynxRAM				;@ Clear RAM
	mov r1,#0x10000/4
	bl memclr_

	bl gfxReset
	bl ioReset
	bl soundReset
	ldmfd sp!,{r4-r11,lr}
	bx lr

;@----------------------------------------------------------------------------

romNum:
	.long 0						;@ romnumber
romInfo:						;@
emuFlags:
	.byte 0						;@ emuflags      (label this so Gui.c can take a peek) see EmuSettings.h for bitfields
//scaling:
	.byte 0						;@ (display type)
	.byte 0,0					;@ (sprite follow val)
cartFlags:
	.byte 0 					;@ cartflags
gConfig:
	.byte 0						;@ Config, bit 7=BIOS on/off
gMachineSet:
	.byte HW_AUTO
gMachine:
	.byte HW_LYNX
gSOC:
	.byte SOC_HOWARD
gLang:
	.byte 1						;@ language
gPaletteBank:
	.byte 0						;@ palettebank
	.byte 0						;@ Game ID
	.byte 0
	.byte 0
	.space 2					;@ alignment.

romSpacePtr:
	.long 0
gRomSize:
romSize:
	.long 0
maxRomSize:
	.long 0
romMask:
	.long 0

#ifdef GBA
	.section .sbss				;@ For the GBA
#else
	.section .bss
#endif
	.align 8
lynxRAM:
	.space 0x10000
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
