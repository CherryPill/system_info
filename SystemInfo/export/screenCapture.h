#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H
#include <Windows.h>
#include "../util/utility.h"
void takeScreenshot(HWND, SCR_SAVETYPE);
void takeScreenShotStub(void);
std::vector<BYTE> convertBitMapToPixels(HBITMAP hBitMap);
#endif // !SCREENCAPTURE_H
