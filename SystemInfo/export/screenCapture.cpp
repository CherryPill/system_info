#include <Windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "atlimage.h"
#include "../export/screenCapture.h"
#include "../util/utility.h"

std::vector<BYTE> convertBitMapToPixels(HBITMAP hBitMap) {
	BITMAP bmp;
	GetObject(hBitMap, sizeof(bmp), &bmp);

	BITMAPINFO info{};
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = bmp.bmWidth;
	// pay attention to the sign, you most likely want a 
	// top-down pixel array as it's easier to use
	info.bmiHeader.biHeight = -bmp.bmHeight;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;

	// the following calculations work for 16/24/32 bits bitmaps 
	// but assume a byte pixel array
	size_t pixelSize = info.bmiHeader.biBitCount / 8;
	// the + 3 ) & ~3 part is there to ensure that each
	// scan line is 4 byte aligned
	size_t scanlineSize = (pixelSize * info.bmiHeader.biWidth + 3) & ~3;
	size_t bitmapSize = bmp.bmHeight * scanlineSize;

	std::vector<BYTE> pixels(bitmapSize);
	GetDIBits(NULL,
		hBitMap,
		0,
		bmp.bmHeight,
		&pixels[0],
		&info, DIB_RGB_COLORS);

	return pixels;
}
void takeScreenshot(HWND hwnd, SCR_SAVETYPE scrSaveType) {
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



	int res = uploadImageToImgur(convertBitMapToPixels(hCaptureBitmap));
	if (res != 0) {
		displayMessageGeneric(UI_MESS_RES::SUCCESS, L"Success");
	}
	int x = 0xf;
	/*
	if (scrSaveType == SCR_SAVETYPE::LOCAL) {
		
	}
	else if(scrSaveType == SCR_SAVETYPE::INTERNET) {
		uploadImageToImgur(convertBitMapToPixels(hCaptureBitmap));
	}
	
	
	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));

	openFileDiag(hwnd, FILE_EXTENSION::PNG, fullSavePath, 1);
	screen.Save(fullSavePath);
	DeleteObject(hCaptureBitmap);
	ReleaseDC(hwnd, mainWindowDC);
	DeleteDC(screenCapture);
	*/
}