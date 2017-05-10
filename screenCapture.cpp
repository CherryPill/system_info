#include <Windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "atlimage.h"
#include "screenCapture.h"
#include "utility.h"
void takeScreenshot(HWND hwnd)
{
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
	//TCHAR *fullSavePath = _T("C:\\Users\\Theta1\\Pictures\\pic.png");
	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));

	openFileDiag(hwnd, FILE_EXTENSION::PNG,fullSavePath);

	//TCHAR saveFileName[256];
	//ZeroMemory(&saveFileName, sizeof(saveFileName));
	//generateFileName(saveFileName);
	/*
	DWORD buffSize = 256;
	TCHAR currentUserName[256];
	ZeroMemory(&currentUserName, sizeof(currentUserName));
	
	bool res = GetUserName(currentUserName, &buffSize);
	_tcscpy(fullSavePath, _T("C:\\Users\\"));
	_tcscat(fullSavePath, currentUserName);
	_tcscat(fullSavePath, _T("\\Pictures\\"));
	_tcscat(fullSavePath, saveFileName);
	_tcscat(fullSavePath, _T(".png"));
	*/
	screen.Save(fullSavePath);
	DeleteObject(hCaptureBitmap);
	ReleaseDC(hwnd, mainWindowDC);
	DeleteDC(screenCapture);
}