#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <vector>
#include "globalVars.h"
HINSTANCE ghInstance = NULL;
HWND mainWindowHwnd = NULL;
int mainWindowWidth = 600;
int mainWindowHeight = 660;
int scrollY = 0;
int scrollFullPageHeight = 0;
HBRUSH grayBrush = CreateSolidBrush(RGB(81, 81, 81));
TCHAR *fontFamilies[] = {
	_T("Arial"),
	_T("Courier New")
};
HFONT appNameFontLarge = CreateFont(-28, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, fontFamilies[0]);

TCHAR *timeVerboseMonths[] = {
	_T("Dummy"),
	_T("January"),
	_T("February"),
	_T("March"),
	_T("April"),
	_T("May"),
	_T("June"),
	_T("July"),
	_T("August"),
	_T("September"),
	_T("October"),
	_T("November"),
	_T("December")
};
TCHAR *timeVerboseDaysOfWeek[] = {
	_T("Sunday"),
	_T("Monday"),
	_T("Tuesday"),
	_T("Wednesday"),
	_T("Thursday"),
	_T("Friday"),
	_T("Saturday")
};