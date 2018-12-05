#define ISOLATION_AWARE_ENABLED 1
#include <windows.h>
#include "resource.h"
#include "glb/globalVars.h"
#include "util/utility.h"
#include "mainWindowProcedure.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {
	int argc;
	//if argcount > 1 then a snapshot is being loaded
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	TCHAR fullExecPath[256] = {0};
	GetModuleFileName(NULL, fullExecPath, 256);
	getFileNameFromPath(fullExecPath, execName);
	_tcscat(execName, L" ");
	if (argc > 1) {
		PROGRAM_INSTANCE = 1;
		_tcscpy(PROGRAM_DATA_IMPORT_LOCATION, argv[1]);
	}
#ifdef _DEBUG
	for (int x = 0; x < argc;x++)
		MessageBox(NULL, argv[x], _T("arg"), MB_OK);
#endif
	LocalFree(argv);
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
	positionWindow(&upperLeftCorner);
	//centerWindow(&upperLeftCorner);
	mainWindowHwnd = CreateWindowEx
		(
			0,
			wc.lpszClassName,
			PROGRAM_INSTANCE?L"SystemInfo (snapshot)":L"SystemInfo",
			WS_SYSMENU|WS_OVERLAPPEDWINDOW,
			upperLeftCorner.x,
			upperLeftCorner.y,
			mainWindowWidth,
			mainWindowHeight,
			NULL,
			NULL,
			hInstance,
			NULL
			);
	SetClassLongPtr(mainWindowHwnd, GCLP_HBRBACKGROUND, (LONG)grayBrush);
	ShowWindow(mainWindowHwnd, nCmdShow);
	UpdateWindow(mainWindowHwnd);
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return (int)(message.wParam);
}