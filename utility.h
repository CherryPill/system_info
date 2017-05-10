#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <tchar.h>
#include "SystemInfo.h"
enum class HARDWARE_VECTOR_TYPE {
	HARDWARE_DISPLAY,
	HARDWARE_STORAGE,
	HARDWARE_VIDEO_ADAPTER,
	HARDWARE_CDROM
};
enum class FILE_EXTENSION {
	TXT,
	XML,
	HTML,
	BIN,
	PNG
};
static std::string storageMediumManufacturers[20]
{
	"Western Digital",
	"Seagate",
	"Fujitsu",
	"Hitachi",
	"Samsung",
	"IMB"
};
void centerWindow(POINT*);
std::string& BstrToStdString(const BSTR, std::string&, int cp = CP_UTF8);
std::wstring& BStrToWStdString(const BSTR bstr, std::wstring& dst, int cp=CP_UTF8);
void trimNullTerminator(std::string&);
std::string parseDiskStorageName(std::string);
std::string convertUIntToString(UINT64);
void trimWhiteSpace(std::string&);
void generateFileName(TCHAR*, FILE_EXTENSION);
UINT32 isAdjustRequired(UINT32,SystemInfo*);
UINT32 adjustItemHeight(HWND, UINT32, UINT32);
void getCurrentDateTime(TCHAR *);
std::string formListString(SystemInfo*,HARDWARE_VECTOR_TYPE);
void openFileDiag(HWND,FILE_EXTENSION,TCHAR*);
#endif