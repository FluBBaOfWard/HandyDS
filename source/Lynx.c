#include <nds.h>

#include "Lynx.h"
#include "LynxBorder.h"
#include "Lynx2Border.h"
#include "Gui.h"
#include "Cart.h"
#include "Gfx.h"


int packState(void *statePtr) {
	int size = 0;
	return size;
}

void unpackState(const void *statePtr) {
	int size = 0;
}

int getStateSize() {
	int size = 0;
	return size;
}

static void setupBorderPalette(const unsigned short *palette, int len) {
	vramSetBankF(VRAM_F_LCD);
	if (gBorderEnable == 0) {
		memset(VRAM_F, 0, len);
	}
	else {
		memcpy(VRAM_F, palette, len);
	}
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT23);
}

void setupLynxBackground() {
	decompress(LynxBorderTiles, BG_TILE_RAM(4), LZ77Vram);
	decompress(LynxBorderMap, BG_MAP_RAM(15), LZ77Vram);
}

void setupLynxBorderPalette() {
	setupBorderPalette(LynxBorderPal, LynxBorderPalLen);
}

void setupLynx2Background() {
	decompress(Lynx2BorderTiles, BG_TILE_RAM(4), LZ77Vram);
	decompress(Lynx2BorderMap, BG_MAP_RAM(15), LZ77Vram);
}

void setupLynx2BorderPalette() {
	setupBorderPalette(Lynx2BorderPal, Lynx2BorderPalLen);
}

void setupEmuBackground() {
	if (gMachine == HW_LYNX_II) {
		setupLynx2Background();
		setupLynx2BorderPalette();
	}
	else {
		setupLynxBackground();
		setupLynxBorderPalette();
	}
}

void setupEmuBorderPalette() {
	if (gMachine == HW_LYNX_II) {
		setupLynx2BorderPalette();
	}
	else {
		setupLynxBorderPalette();
	}
}
