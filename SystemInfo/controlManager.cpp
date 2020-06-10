#include "controlManager.h"
HWND ControlManager::appCreateControl(
	TCHAR *className, 
	int xOffset, 
	int yOffSet, 
	int width, 
	int height, 
	std::bitset<32> windowStyles,
	HWND parent,
	int controlMenuID,
	TCHAR* windowName,
	DWORD exStyle,
	HINSTANCE hInstance,
	LPVOID lParam
	) {
	HWND createdWindow = CreateWindowEx(
		0,
		className,
		windowName,
		windowStyles.to_ulong(),
		xOffset,
		yOffSet,
		width,
		height,
		parent,
		(HMENU)controlMenuID,
		hInstance,
		lParam
	);
	if (!createdWindow) {
		DWORD errCode = GetLastError();

		//to do: implement this https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

		int s = 11;
	}
	return createdWindow;
}

bool ControlManager::searchAndDestroy(
	HWND parentWindowHandle, 
	TCHAR *className,
	TCHAR *windowName) {
	HWND foundWindowHandle = NULL;
	if ((foundWindowHandle = FindWindowEx(NULL, NULL, className, windowName)) != NULL) {
		return DestroyWindow(foundWindowHandle);
	}
	return false;
}