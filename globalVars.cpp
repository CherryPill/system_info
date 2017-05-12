#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <vector>
#include "globalVars.h"
HINSTANCE ghInstance = NULL;
HWND mainWindowHwnd = NULL;
int mainWindowWidth = 600;
int mainWindowHeight = 600;
HBRUSH grayBrush = CreateSolidBrush(RGB(81, 81, 81));
TCHAR *fontFamilies[] =
{
	_T("Arial"),
	_T("Courier New")
};
HFONT appNameFontLarge = CreateFont(-28, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, fontFamilies[0]);
