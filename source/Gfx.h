#ifndef GFX_HEADER
#define GFX_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "ARMSuzy/ARMSuzy.h"

extern u8 gFlicker;
extern u8 gTwitch;
extern u8 gGfxMask;

extern u16 EMUPALBUFF[0x200];
extern u32 GFX_DISPCNT;
extern u16 GFX_BG0CNT;
extern u16 GFX_BG1CNT;

void gfxInit(void);
void vblIrqHandler(void);
void monoPalInit(void);
void paletteInit(u8 gammaVal);
void updateLCDRefresh(void);
void gfxRefresh(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GFX_HEADER
