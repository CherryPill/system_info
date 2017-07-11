#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <fstream>
#include <tchar.h>
#include "SystemInfo.h"
enum class FILE_IO_OPERATION {
	SAVE_AS,
	OPEN
};
static wstring ipToggleText[]{
		L"Hide IP",
		L"Show IP"
};
static wstring writeOutPrefix[] {
	L"",
	L"",
	L"",
	L"<subitem>"
};
static wstring writeOutPostfix[]{
	L"\n",
	L"\n\t",
	L"<br />",
	L"</subitem>"
};
enum class WRITE_OUT_TYPE {
	APP_WINDOW,
	FILE_TXT,
	FILE_HTML,
	FILE_XML
};
enum class HARDWARE_VECTOR_TYPE {
	HARDWARE_VIDEO_ADAPTER=0,
	HARDWARE_DISPLAY=1,
	HARDWARE_STORAGE=2,
	HARDWARE_CDROM=3,
	HARDWARE_NETWORK=4
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
void positionWindow(POINT*);
std::string& BstrToStdString(const BSTR, std::string&, int cp = CP_UTF8);
void trimNullTerminator(std::wstring&);
std::wstring parseDiskStorageName(std::wstring);
std::wstring convertUIntToString(UINT64);
void trimWhiteSpace(std::wstring&);
void generateFileName(TCHAR*, FILE_EXTENSION);
UINT32 getInfoBoxItemCount(UINT32,SystemInfo*);
UINT32 adjustItemHeight(HWND, UINT32, UINT32);
void getCurrentDateTime(TCHAR *);
void getCurrentDateTimeVerbose(TCHAR *);
std::wstring formListString(SystemInfo*,HARDWARE_VECTOR_TYPE, WRITE_OUT_TYPE);
void openFileDiag(HWND,FILE_EXTENSION,TCHAR*, int);
void writeToFile(wofstream&, SystemInfo*, int, WRITE_OUT_TYPE);
wstring fromChToWideStr(char *value);
wstring fromIntToWideStr(int);
std::wstring convertStringToWide(const std::string& as);
void prependMinuteStr(WORD min, TCHAR *minBuff);
vector<wstring> stringSplit(const wchar_t *s, wchar_t delimiter);
std::wstring netAdapterStringWrapper(NetAdapter);
void getFileNameFromPath(TCHAR *fullPath, TCHAR *fileName);
bool isMultiSlot(int);
#endif