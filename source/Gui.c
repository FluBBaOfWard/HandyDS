#include <nds.h>

#include "Gui.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Main.h"
//#include "SVController.h"
#include "FileHandling.h"
#include "Cart.h"
#include "Gfx.h"
#include "io.h"
#include "cpu.h"

#define EMUVERSION "V0.0.1 2024-10-13"

static void gammaChange(void);
static void paletteChange(void);
static const char *getPaletteText(void);
static void machineSet(void);
static const char *getMachineText(void);
//static void speedHackSet(void);
//static const char *getSpeedHackText(void);
static void refreshChgSet(void);
static const char *getRefreshChgText(void);
static void borderSet(void);
static const char *getBorderText(void);
static void swapABSet(void);
static const char *getSwapABText(void);
static void contrastSet(void);
static const char *getContrastText(void);

const MItem dummyItems[] = {
	{"", uiDummy}
};
const MItem fileItems[] = {
	{"Load Game", selectGame},
	{"Load State", loadState},
	{"Save State", saveState},
	{"Save Settings", saveSettings},
	{"Eject Game", ejectGame},
	{"Reset Console", resetGame},
	{"Quit Emulator", ui9}
};
const MItem optionItems[] = {
	{"Controller", ui4},
	{"Display", ui5},
	{"Machine", ui6},
	{"Settings", ui7},
	{"Debug", ui8}
};
const MItem ctrlItems[] = {
	{"B Autofire:", autoBSet, getAutoBText},
	{"A Autofire:", autoASet, getAutoAText},
	{"Swap A-B:  ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Gamma:", gammaChange, getGammaText},
	{"Contrast:", contrastSet, getContrastText},
	{"B&W Palette:", paletteChange, getPaletteText},
	{"Border:", borderSet, getBorderText},
};
const MItem machineItems[] = {
	{"Machine:", machineSet, getMachineText},
};
const MItem setItems[] = {
	{"Speed:", speedSet, getSpeedText},
	{"Allow Refresh Change:", refreshChgSet, getRefreshChgText},
	{"Autoload State:", autoStateSet, getAutoStateText},
	{"Autosave Settings:", autoSettingsSet, getAutoSettingsText},
	{"Autopause Game:", autoPauseGameSet, getAutoPauseGameText},
	{"Powersave 2nd Screen:", powerSaveSet, getPowerSaveText},
	{"Emulator on Bottom:", screenSwapSet, getScreenSwapText},
	{"Autosleep:", sleepSet, getSleepText},
};
const MItem debugItems[] = {
	{"Debug Output:", debugTextSet, getDebugText},
	{"Step Frame", stepFrame},
};
const MItem quitItems[] = {
	{"Yes ", exitEmulator},
	{"No ", backOutOfMenu},
};

const Menu menu0 = MENU_M("", uiNullNormal, dummyItems);
Menu menu1 = MENU_M("", uiAuto, fileItems);
const Menu menu2 = MENU_M("", uiAuto, optionItems);
const Menu menu3 = MENU_M("", uiAbout, dummyItems);
const Menu menu4 = MENU_M("Controller Settings", uiAuto, ctrlItems);
const Menu menu5 = MENU_M("Display Settings", uiAuto, displayItems);
const Menu menu6 = MENU_M("Machine Settings", uiAuto, machineItems);
const Menu menu7 = MENU_M("Settings", uiAuto, setItems);
const Menu menu8 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu9 = MENU_M("Quit Emulator?", uiAuto, quitItems);
const Menu menu10 = MENU_M("", uiDummy, dummyItems);

const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6, &menu7, &menu8, &menu9, &menu10 };

u8 gContrastValue = 1;
u8 gBorderEnable = 1;

const char *const machTxt[]  = {"Auto", "Lynx", "LynxII", "Proto"};
const char *const bordTxt[]  = {"Black", "Border Color", "None"};
const char *const palTxt[]   = {"Classic", "Black & White", "Red", "Green", "Blue", "Green-Blue", "Blue-Green", "Puyo Puyo Tsu"};


void setupGUI() {
	emuSettings = AUTOPAUSE_EMULATION | AUTOSLEEP_OFF;
	keysSetRepeat(25, 4);	// delay, repeat.
	menu1.itemCount = ARRSIZE(fileItems) - (enableExit?0:1);
	openMenu();
}

/// This is called when going from emu to ui.
void enterGUI() {
}

/// This is called going from ui to emu.
void exitGUI() {
}

void quickSelectGame(void) {
	openMenu();
	selectGame();
	closeMenu();
}

void uiNullNormal() {
	uiNullDefault();
}

void uiAbout() {
	cls(1);
	drawTabs();
	drawMenuText("B:          Lynx B button", 4, 0);
	drawMenuText("A:          Lynx A button", 5, 0);
	drawMenuText("Y:          Lynx Option I", 6, 0);
	drawMenuText("X:          Lynx Option II", 7, 0);
	drawMenuText("Start:      Lynx Start button", 8, 0);

	drawMenuText("HandyDS      " EMUVERSION, 23, 0);
}

void nullUINormal(int key) {
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void nullUIDebug(int key) {
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void ejectGame() {
	ejectCart();
}

void resetGame() {
	checkMachine();
	loadCart();
}

//---------------------------------------------------------------------------------
void debugIO(u8 port, u8 val, const char *message) {
	char debugString[32];

	debugString[0] = 0;
	strlcat(debugString, message, sizeof(debugString));
	char2HexStr(&debugString[strlen(debugString)], port);
	strlcat(debugString, " val:", sizeof(debugString));
	char2HexStr(&debugString[strlen(debugString)], val);
	debugOutput(debugString);
}
//---------------------------------------------------------------------------------
void debugIOUnimplR(u8 port) {
	debugIO(port, 0, "Unimpl R port:");
}
void debugIOUnimplW(u8 port, u8 val) {
	debugIO(port, val, "Unimpl W port:");
}
void debugIOUnmappedR(u8 port) {
	debugIO(port, 0, "Unmapped R port:");
}
void debugIOUnmappedW(u8 port, u8 val) {
	debugIO(port, val, "Unmapped W port:");
}
void debugUndefinedInstruction() {
	debugOutput("Undefined Instruction.");
}
void debugCrashInstruction() {
	debugOutput("CPU Crash!");
}

//---------------------------------------------------------------------------------
/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}
const char *getSwapABText() {
	return autoTxt[(joyCfg>>10)&1];
}

/// Change gamma (brightness)
void gammaChange() {
	gammaSet();
	paletteInit(gGammaValue);
	setupMenuPalette();
}

/// Change contrast
void contrastSet() {
	gContrastValue++;
	if (gContrastValue > 4) gContrastValue = 0;
	paletteInit(gGammaValue);
	settingsChanged = true;
}
const char *getContrastText() {
	return brighTxt[gContrastValue];
}

void paletteChange() {
	gPaletteBank++;
	if (gPaletteBank > 4) {
		gPaletteBank = 0;
	}
	monoPalInit();
	paletteInit(gGammaValue);
	settingsChanged = true;
}
const char *getPaletteText() {
	return palTxt[gPaletteBank];
}

void borderSet() {
	gBorderEnable ^= 0x01;
	setupEmuBorderPalette();
}
const char *getBorderText() {
	return bordTxt[gBorderEnable];
}

void machineSet() {
	gMachineSet++;
	if (gMachineSet >= HW_SELECT_END) {
		gMachineSet = 0;
	}
}
const char *getMachineText() {
	return machTxt[gMachineSet];
}

void refreshChgSet() {
	emuSettings ^= ALLOW_REFRESH_CHG;
	updateLCDRefresh();
}
const char *getRefreshChgText() {
	return autoTxt[(emuSettings&ALLOW_REFRESH_CHG)>>19];
}
