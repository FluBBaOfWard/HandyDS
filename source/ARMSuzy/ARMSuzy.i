//
//  SVVideo.i
//  Watara Supervision video emulation for GBA/NDS.
//
//  Created by Fredrik Ahlström on 2004-11-30.
//  Copyright © 2004-2024 Fredrik Ahlström. All rights reserved.
//
;@ ASM header for the Watara Supervision video emulator

#define HW_AUTO       (0)
#define HW_LYNX       (1)
#define HW_LYNX_II    (2)
#define HW_SELECT_END (3)

#define SOC_HOWARD    (0)
#define SOC_HOWARD2   (1)

/** Game screen width in pixels */
#define GAME_WIDTH  (160)
/** Game screen height in pixels */
#define GAME_HEIGHT (102)

	suzptr		.req r12
						;@ SVVideo.s
	.struct 0

windowData:			.long 0
suzyState:					;@
svvRegs:




suzySize:

;@----------------------------------------------------------------------------

