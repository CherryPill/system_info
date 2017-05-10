#ifndef GLOBALVARS_H
#define GLOBALVARS_H
#include <windows.h>
#include <iostream>
#include <vector>
#include <tchar.h>
#include "resource.h"
using namespace std;
extern HINSTANCE ghInstance;
extern HWND mainWindowHwnd;
extern int mainWindowWidth;
extern int mainWindowHeight;
extern HBRUSH grayBrush;
static vector<HICON> iconArr;

static WORD ICON_IDS[10]
{

	OS_ICON,
	CPU_ICON,
	MB_ICON,
	RAM_ICON,
	VIDEO_ICON,
	DISPLAY_ICON,
	STORAGE_ICON,
	OPTICAL_ICON,
	AUDIO_ICON,
	UPTIME_ICON
};
static string itemStrings[10] =
{
	"Operating System",
	"CPU",
	"Motherboard",
	"RAM",
	"GPU",
	"Display",
	"Storage",
	"Optical drives",
	"Sound",
	"Uptime"
};
static TCHAR *savefileExtensions[5] =
{
	_T(".txt"),
	_T(".xml"),
	_T(".html"),
	_T(".sysinfo"),
	_T(".png")
};
static TCHAR *savefileExtensionsLong[5] = 
{
	_T(".txt files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0"),
	_T(".xml files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0"),
	_T(".html files (*.html)\0*.html\0All Files (*.*)\0*.*\0"),
	_T(".sysinfo files (*.sysinfo)\0*.sysinfo\0All Files (*.*)\0*.*\0"),
	_T(".png images (*.png)\0*.png\0All Files (*.*)\0*.*\0")
};
static string RAMFormFactors[24]
{
	"Unknown form factor",
	"",
	"SIP",
	"DIP",
	"ZIP",
	"SOJ",
	"Proprietary",
	"SIMM",
	"DIMM",
	"TSOP",
	"PGA",
	"RIMM",
	"SODIMM",
	"SRIMM",
	"SMD",
	"SSMP",
	"QFP",
	"TQFP",
	"SOIC",
	"LCC",
	"PLCC",
	"BGA",
	"FPBGA",
	"LGA"
};
static string RAMMemoryTypes[26]
{
	"DDR3",
	"",
	"SDRAM",
	"Cache DRAM",
	"EDO",
	"EDRAM",
	"VRAM",
	"SRAM",
	"RAM",
	"ROM",
	"Flash",
	"EEPROM",
	"FEPROM",
	"CDRAM",
	"3DRAM",
	"SDRAM",
	"SGRAM",
	"RDRAM",
	"DDR",
	"DDR2",
	"DDR2 FB-DIMM",
	"DDR2",
	"DDR3",
	"FBD2"
};
#endif