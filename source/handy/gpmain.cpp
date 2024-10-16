#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "system.h"
#include "../Shared/FileHelper.h"
#include "../io.h"

extern "C" void GpInit(const unsigned char *gamerom, int size);
extern "C" void GpDelete(void);
extern "C" void GpMain(void *args);


const char *romfile = "LYNXBOOT.IMG";

ULONG mColourMap[4096];
unsigned short vram[2][160*102];
unsigned short *currentDest;
int bufIdx = 0;
bool gScreenUpdateRequired = false;

CSystem *newsystem = NULL;

int loadFile(const char *fname, void *dest, int start, int maxSize) {
	FILE *fHandle;

	findFolder("Lynx");

	if ( !(fHandle = fopen(fname, "r" ))) {
		printf("Couldn't load file **1**\n");
		return -1;
	}
	if (start > 0) {
		fseek(fHandle, start, SEEK_SET);
	}
	int size = fread(dest, 1, maxSize, fHandle);
	fclose(fHandle);
	return size;
}

void handy_nds_render_callback(UBYTE *ram, ULONG *palette, bool flip) {
	UWORD *bitmap_tmp = currentDest;
	for (int loop=0;loop<LYNX_SCREEN_WIDTH/2;loop++) {
		int source = *ram;
		if (flip) {
			ram -= 1;
			*bitmap_tmp = (UWORD)mColourMap[palette[source & 0x0f]];
			bitmap_tmp += 1;
			*bitmap_tmp = (UWORD)mColourMap[palette[source >> 4]];
			bitmap_tmp += 1;
		}
		else {
			ram += 1;
			*bitmap_tmp = (UWORD)mColourMap[palette[source >> 4]];
			bitmap_tmp += 1;
			*bitmap_tmp = (UWORD)mColourMap[palette[source & 0x0f]];
			bitmap_tmp += 1;
		}
	}
	currentDest += 256;
}

void handy_nds_display_callback(void)
{
	bufIdx ^= 1;
	currentDest = ((unsigned short *)0x06000000);
	gScreenUpdateRequired = TRUE;
}


void GpInit(const unsigned char *gamerom, int size) {
	newsystem = new CSystem(gamerom, size, HANDY_FILETYPE_LNX, romfile);
	newsystem->DisplaySetAttributes(handy_nds_display_callback, handy_nds_render_callback);
	currentDest = ((unsigned short *)0x06000000);
	for (int i=0;i<4096;i++) {
		mColourMap[i] = ((i<<3) & 0x7c00) | 0x8000;
		mColourMap[i] |= (i<<6) & 0x03e0;
		mColourMap[i] |= (i>>3) & 0x001f;
	}
}

void GpDelete() {
	if (newsystem != NULL) {
		delete newsystem;
		newsystem = NULL;
	}
}

void GpMain(void *args) {

	int hazard = 0;
	while (newsystem != NULL) {

		for (int i=0;i<1024;i++) {
			newsystem->Update();
		}
		newsystem->SetButtonData( joy0_R() );

		hazard += 1;
		if (gScreenUpdateRequired || hazard > 30) {
			gScreenUpdateRequired = FALSE;
			return;
		}
	}

}
