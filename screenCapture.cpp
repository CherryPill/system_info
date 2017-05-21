#include <Windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "atlimage.h"
#include "screenCapture.h"
#include "utility.h"
void takeScreenshot(HWND hwnd) {
	RECT winSize;
	GetClientRect(hwnd,&winSize);
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

	openFileDiag(hwnd, FILE_EXTENSION::PNG,fullSavePath);
	screen.Save(fullSavePath);
	DeleteObject(hCaptureBitmap);
	ReleaseDC(hwnd, mainWindowDC);
	DeleteDC(screenCapture);
}