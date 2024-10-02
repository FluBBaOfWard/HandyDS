
#ifdef __arm__

#include "ARMSuzy.i"

	.global svRefW

	.syntax unified
	.arm

#if GBA
	.section .ewram, "ax", %progbits	;@ For the GBA
#else
	.section .text						;@ For anything else
#endif
	.align 2
;@----------------------------------------------------------------------------
svRefW:						;@ 0x2001, Last scan line.
;@----------------------------------------------------------------------------
//	strb r1,[suzptr,#svvLCDVSize]
	cmp r1,#0x87
	movmi r1,#0x87
	cmp r1,#0xC8
	movpl r1,#0xC8
	add r1,r1,#1
	str r1,lineStateLastLine
	mov r0,r1
	b setScreenRefresh

	bl memRead8

lineStateLastLine:
	.long 160

#endif // #ifdef __arm__
