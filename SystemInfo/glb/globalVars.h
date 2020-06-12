#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include <windows.h>
#include <iostream>
#include <vector>
#include <tchar.h>
#include <unordered_map>
#include "../meta/SoftwareInfo.h"	
#include "../resource.h"

using namespace std;
extern int PROGRAM_INSTANCE; //0 - parent, 1 - child
extern TCHAR PROGRAM_DATA_IMPORT_LOCATION[256]; //path to the xml file to import data from if the process is spawned by parent
extern TCHAR execName[256];
extern SoftwareInfo *shippedSoftWare;
extern HINSTANCE ghInstance;
extern HWND mainWindowHwnd;
extern int mainWindowWidth;
extern int mainWindowHeight;
extern int scrollY;
extern int scrollFullPageHeight;
extern HBRUSH grayBrush;
extern HFONT appNameFontLarge;
static vector<HICON> iconArr;
static vector<HICON> iconArrCpuUtilizationIcons;
const static int iconArrCpuUtilizationIconsSize = 6;
const static int totalItemsCount = 13;


static int glbCpuProgressStringXOffset;
static int glbCpuProgressStringYOffset;
const static int cpuProgressStringWindowWidthSmall = 26;

const static int cpuProgressStringWindowHeightSmall = 15;
const static int cpuProgressStringWindowWidthLarge = 40;
const static int cpuProgressStringWindowHeightLarge = 15;

static WORD ICON_IDS[totalItemsCount]{
	BIOS_ICON,
	OS_ICON,
	CPU_ICON,
	MB_ICON,
	RAM_ICON,
	VIDEO_ICON,
	DISPLAY_ICON,
	STORAGE_ICON,
	OPTICAL_ICON,
	NETWORK_ICON,
	AUDIO_ICON,
	UPTIME_ICON
};

static WORD UTIL_iCON_IDS[iconArrCpuUtilizationIconsSize]{
	UTILIZATION_ICON_0,
	UTILIZATION_ICON_1,
	UTILIZATION_ICON_2,
	UTILIZATION_ICON_3,
	UTILIZATION_ICON_4,
	UTILIZATION_ICON_5
};



static wstring UI_messagesCapt[] = {
	L"Success",
	L"Error"
};

static wstring itemStrings[totalItemsCount] = {
	L"BIOS/UEFI",
	L"Operating System",
	L"CPU",
	L"Motherboard",
	L"RAM",
	L"GPU",
	L"Display",
	L"Storage",
	L"Optical drives",
	L"Network and Connections",
	L"Sound",
	L"Uptime",
	L"Snapshot"
};
//DO NOT change, a map depends on it
static TCHAR* savefileExtensions[] = {
	_T(".txt"),
	_T(".xml"),
	_T(".html"),
	_T(".png")
};

static TCHAR *savefileExtensionsLong[] = {
	_T(".txt files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"),
	_T(".xml files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0"),
	_T(".html files (*.html)\0*.html\0All Files (*.*)\0*.*\0"),
	_T(".png images (*.png)\0*.png\0All Files (*.*)\0*.*\0")
};

static wstring RAMFormFactors[24]{
	L"Unknown form factor",
	L"",
	L"SIP",
	L"DIP",
	L"ZIP",
	L"SOJ",
	L"Proprietary",
	L"SIMM",
	L"DIMM",
	L"TSOP",
	L"PGA",
	L"RIMM",
	L"SODIMM",
	L"SRIMM",
	L"SMD",
	L"SSMP",
	L"QFP",
	L"TQFP",
	L"SOIC",
	L"LCC",
	L"PLCC",
	L"BGA",
	L"FPBGA",
	L"LGA"
};

static wstring RAMMemoryTypes[] = {
	L"Unknown",
	L"Other",
	L"DRAM",
	L"Synchronous DRAM",
	L"Cache DRAM",
	L"EDO",
	L"EDRAM",
	L"VRAM",
	L"SRAM",
	L"RAM",
	L"ROM",
	L"Flash",
	L"EEPROM",
	L"FEPROM",
	L"EPROM",
	L"CDRAM",
	L"3DRAM",
	L"SDRAM",
	L"SGRAM",
	L"RDRAM",
	L"DDR",
	L"DDR2",
	L"DDR2 FB-DIMM",
	L"",
	L"DDR3",
	L"FBD2",
	L"DDR4",
	L"LPDDR",
	L"LPDDR2",
	L"LPDDR3",
	L"LPDDR4",
	L"Logical non-volatile device",
	L"HBM",
	L"HBM2"
};

static wstring PCType[2]{
	L" (Laptop) ",
	L" (Desktop) "
};

//time functions
extern TCHAR *timeVerboseMonths[];
extern TCHAR *timeVerboseDaysOfWeek[];
//font
extern HFONT ipToggleBtnFont;

enum class ACTION {
	ACCEPTED,
	CANCELED_OUT,
	__ERROR
};

extern WNDPROC lpfnScrEditProc;
extern TCHAR sysInfoConfigDirectoryPath[256];
extern int currentCpuUsageGlobal;

extern int glbCpuInfoHolderXoffset;
extern int glbCpuInfoHolderYoffset;

#endif