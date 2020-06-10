#pragma once
#include <Windows.h>
#include <bitset>
class ControlManager {
public:
	static HWND appCreateControl(
		TCHAR* className,
		int xOffset,
		int yOffSet,
		int width,
		int height,
		std::bitset<32> windowStyles,
		HWND parent,
		int controlMenuID,
		TCHAR* windowName = NULL,
		DWORD exStyle = 0,
		HINSTANCE hInstance = NULL,
		LPVOID lParam = NULL);
	static bool searchAndDestroy(
		HWND parentWindowHandle, 
		TCHAR *className,
		TCHAR *windowName);
};