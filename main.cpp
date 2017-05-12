#define ISOLATION_AWARE_ENABLED 1
#include <windows.h>
#include "resource.h"
#include "globalVars.h"
#include "utility.h"
#include "mainWindowProcedure.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {
	shippedSoftWare = new SoftwareInfo();
	MSG message;
	WNDCLASSEX wc = {0}; //nullifying the struct
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"sysinfo";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpfnWndProc = mainWindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,0,0,NULL);
	wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, NULL);
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window class registration failed", L"Error", MB_ICONERROR);
		return FALSE;
	}
	ghInstance = hInstance;
	POINT upperLeftCorner;
	centerWindow(&upperLeftCorner);
	mainWindowHwnd = CreateWindowEx
		(
			0,
			wc.lpszClassName,
			L"SystemInfo",
			WS_SYSMENU|WS_MINIMIZEBOX|WS_VSCROLL,
			upperLeftCorner.x,
			upperLeftCorner.y,
			mainWindowWidth,
			mainWindowHeight,
			NULL,
			NULL,
			hInstance,
			NULL
			);
	SCROLLINFO scInfo;
	scInfo.cbSize = sizeof(SCROLLINFO);
	scInfo.nMin = 0;
	scInfo.nMax = mainWindowHeight*2;
	scInfo.fMask = SIF_RANGE;
	SetScrollInfo(mainWindowHwnd,SB_VERT, &scInfo, TRUE);
	SetClassLongPtr(mainWindowHwnd, GCLP_HBRBACKGROUND, (LONG)grayBrush);
	ShowWindow(mainWindowHwnd, nCmdShow);
	UpdateWindow(mainWindowHwnd);
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return (int)(message.wParam);
}