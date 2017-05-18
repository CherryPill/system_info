#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <fstream>
#include <tchar.h>
#include "SystemInfo.h"
enum class WRITE_OUT_TYPE {
	APP_WINDOW,
	FILE
};
enum class HARDWARE_VECTOR_TYPE {
	HARDWARE_VIDEO_ADAPTER,
	HARDWARE_DISPLAY,
	HARDWARE_STORAGE,
	HARDWARE_CDROM
};
enum class WINDOW_CENTER_TYPE {
	CENTER_MAIN,
	CENTER_CHILD
};
enum class FILE_EXTENSION {
	TXT,
	XML,
	HTML,
	BIN,
	PNG
};
static std::wstring storageMediumManufacturers[20]
{
	L"Western Digital",
	L"Seagate",
	L"Fujitsu",
	L"Hitachi",
	L"Samsung",
	L"IMB"
};
void centerWindow(POINT*);
std::string& BstrToStdString(const BSTR, std::string&, int cp = CP_UTF8);
void trimNullTerminator(std::wstring&);
std::wstring parseDiskStorageName(std::wstring);
std::wstring convertUIntToString(UINT64);
void trimWhiteSpace(std::wstring&);
void generateFileName(TCHAR*, FILE_EXTENSION);
UINT32 isAdjustRequired(UINT32,SystemInfo*);
UINT32 adjustItemHeight(HWND, UINT32, UINT32);
void getCurrentDateTime(TCHAR *);
void getCurrentDateTimeVerbose(TCHAR *);
std::wstring formListString(SystemInfo*,HARDWARE_VECTOR_TYPE, WRITE_OUT_TYPE);
void openFileDiag(HWND,FILE_EXTENSION,TCHAR*);
void writeToFile(wofstream&, SystemInfo*, int);
#endif