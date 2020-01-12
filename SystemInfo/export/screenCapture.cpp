#include <Windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "atlimage.h"
#include "../export/screenCapture.h"
#include "../util/utility.h"
#include "../dialog/scrUploadDialog.h"
#include "../network/rest/rest.h"

ACTION takeScreenshot(HWND hwnd, SCR_SAVETYPE scrSaveType, RESULT_STRUCT *res) {
	RECT winSize;
	GetClientRect(hwnd, &winSize);
	INT32 areaWidth = winSize.right - winSize.left;
	INT32 areaHeight = winSize.bottom - winSize.top;
	HDC mainWindowDC = GetDC(hwnd);
	HDC screenCapture = CreateCompatibleDC(mainWindowDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(mainWindowDC,
													areaWidth, areaHeight);
	SelectObject(screenCapture, hCaptureBitmap);
	BitBlt(screenCapture, 0, 0, areaWidth, areaHeight,
		   mainWindowDC, 0, 0, SRCCOPY | CAPTUREBLT);
	CImage screen;
	
	screen.Attach(hCaptureBitmap);

	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));
	if (scrSaveType == SCR_SAVETYPE::INTERNET) {
		_tcscpy(fullSavePath, sysInfoConfigDirectoryPath);
		TCHAR uuidString[256];
		_tcscat(fullSavePath, L"\\");
		_tcscat(fullSavePath, L"_temp.png");
	} else if (scrSaveType == SCR_SAVETYPE::LOCAL) {
		if (openFileDiag(hwnd, FILE_EXTENSION::PNG, fullSavePath, 1) == ACTION::CANCELED_OUT) {
			return ACTION::CANCELED_OUT;
		}
	}

	if (!SUCCEEDED(screen.Save(fullSavePath))) {
		return ACTION::__ERROR;
	}
	DeleteObject(hCaptureBitmap);
	ReleaseDC(hwnd, mainWindowDC);
	DeleteDC(screenCapture);
	if (scrSaveType == SCR_SAVETYPE::LOCAL) {
		res->src.assign(fullSavePath);
	}
	if (scrSaveType == SCR_SAVETYPE::INTERNET) {
		RESULT_STRUCT resStruct = {};
		if (!uploadImage(&resStruct, fullSavePath)) {
			MessageBox(NULL, _T("Failed to upload"), NULL, MB_ICONERROR | MB_OK);
			return ACTION::__ERROR;
		}
		else {
			_tcscpy(uploadSrcLinkData.link, resStruct.__src);
		}
		DeleteFile(fullSavePath);
	}
	return ACTION::ACCEPTED;
}