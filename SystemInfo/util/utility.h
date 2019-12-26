#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <fstream>
#include <tchar.h>
#include "../core/SystemInfo.h"
enum class UI_MESS_RES {
	SUCCESS,
	FAILURE
};

enum class UI_MESS_ACTION {
	WRITE_OUT_TXT,
	WRITE_OUT_XML,
	WRITE_OUT_HTML
};

enum class FILE_IO_OPERATION {
	SAVE_AS,
	OPEN
};

static wstring ipToggleText[]{
	L"Hide IP",
	L"Show IP"
};

static wstring writeOutPrefix[]{
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

enum class OS_INFO_TYPES {
	MIDDLEWARE_BIOS = 1,
	SOFTWARE_OS,
	HARDWARE_MB,
	HARDWARE_RAM,
	HARDWARE_VIDEO_ADAPTER,
	HARDWARE_DISPLAY,
	HARDWARE_STORAGE,
	HARDWARE_CDROM,
	HARDWARE_NETWORK
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

enum class SCR_SAVETYPE {
	LOCAL,
	INTERNET
};
static std::wstring storageMediumManufacturers[20]{
	L"Western Digital",
	L"Seagate",
	L"Fujitsu",
	L"Hitachi",
	L"Samsung",
	L"IMB"
};

typedef struct RESULT_STRUCT {
	bool result;
	std::wstring src;
	TCHAR __src[256];
};

static std::wstring actionPromptText[]{
	L"Open exported file?"
};

void centerWindow(POINT*);
void positionWindow(POINT*);
std::string& BstrToStdString(const BSTR, std::string&, int cp = CP_UTF8);
void trimNullTerminator(std::wstring&);
std::wstring parseDiskStorageName(std::wstring);
std::wstring convertUIntToString(UINT64);
void trimWhiteSpace(std::wstring&);
void generateFileName(TCHAR*, FILE_EXTENSION);
UINT32 getInfoBoxItemCount(UINT32, SystemInfo*);
UINT32 adjustItemHeight(HWND, UINT32, UINT32);
void getCurrentDateTime(TCHAR *);
void getCurrentDateTimeVerbose(TCHAR *);
std::wstring formListString(SystemInfo*, OS_INFO_TYPES, WRITE_OUT_TYPE);
enum class ACTION openFileDiag(HWND, FILE_EXTENSION, TCHAR*, int);
void writeToFile(wofstream&, SystemInfo*, int, WRITE_OUT_TYPE);
wstring fromChToWideStr(char *value);
wstring fromIntToWideStr(int);
std::wstring convertStringToWide(const std::string& as);
void prependMinuteStr(WORD min, TCHAR *minBuff);
vector<wstring> stringSplit(const wchar_t *s, wchar_t delimiter);
std::wstring netAdapterStringWrapper(NetAdapter);
void getFileNameFromPath(TCHAR *fullPath, TCHAR *fileName);
bool fileIOCheck(wofstream&);
void calculateTimeAndFormat(TCHAR*);
void displayExportMessage(enum class UI_MESS_RES, enum class UI_MESS_ACTION);
void displayMessageGeneric(enum class UI_MESS_RES, const TCHAR*);
UI_MESS_ACTION getUIMessByCommand(WORD);
int displayPromptForAction(std::wstring);
BOOL openDefAppForExpData(WORD command, RESULT_STRUCT *res);
void configAppData();
bool dirExists(LPCTSTR);
std::wstring convertWmiCapacityToGB(std::wstring);
#endif