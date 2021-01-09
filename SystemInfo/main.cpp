#define ISOLATION_AWARE_ENABLED 1
#include <windows.h>
#include "resource.h"
#include "glb/globalVars.h"
#include "util/utility.h"
#include "mainWindowProcedure.h"
#include "settings/settings.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow) {
	int argc;
	//if argcount > 1 then a snapshot is being loaded
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

#ifdef _DEBUG
	for (int x = 0; x < argc; x++)
		MessageBox(NULL, argv[x], _T("arg"), MB_OK);
#endif
	TCHAR fullExecPath[256] = { 0 };
	GetModuleFileName(NULL, fullExecPath, 256);
	getFileNameFromPath(fullExecPath, execName);
	_tcscat(execName, L" ");
	configAppData();
	std::unique_ptr<SavedUserSettings> savedUserSettings(new SavedUserSettings());
	SavedUserSettingsHelper::initializeFullConfigFilePath();
	if (!SavedUserSettingsHelper::loadSettingsFromDisk(savedUserSettings.get())) {
		SavedUserSettingsHelper::saveSettingsToDisk(savedUserSettings.get());
	}
	SavedUserSettingsHelper::fillSettingsCheckBoxState(savedUserSettings.get());
	SavedUserSettingsHelper::fillSettingsComboBoxState(savedUserSettings.get());

	glbUserSettings = savedUserSettings.get();
	//savedUserSettings.get()->setShowHDDTemp(true);
	if (NULL != argv[1] &&_tcscmp(argv[1], L"import") == 0) {
		applicationOpMode = APPLICATION_OPERATION_MODE::SNAPSHOT;
		_tcscpy(PROGRAM_DATA_IMPORT_LOCATION, argv[2]);
	}
	else {
		if (NULL == argv[1] || _tcscmp(L"as_admin", argv[1]) != 0) {
			applicationOpMode = APPLICATION_OPERATION_MODE::PARENT_NON_ADMIN;
			if (savedUserSettings.get()->getShowHDDTemp()) {
				if (ShellExecute(NULL, L"runas", fullExecPath, L"as_admin", NULL, SW_SHOWNORMAL) < (HINSTANCE)32) {
					MessageBox(NULL, L"Unable to start program as admin.", NULL, MB_OK);
				}
				ExitProcess(0);
			}
		}
		else {
			applicationOpMode = APPLICATION_OPERATION_MODE::PARENT_ADMIN;
		}
	}
	LocalFree(argv);
	shippedSoftWare = new SoftwareInfo();
	MSG message;
	WNDCLASSEX wc = { 0 }; //nullifying the struct
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
	wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(MAIN_APP_ICON), IMAGE_ICON, 0, 0, NULL);
	wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(MAIN_APP_ICON), IMAGE_ICON, 32, 32, NULL);
	if (!RegisterClassEx(&wc)) {
		GenericMessageOK()
			.withMessage(L"Window class registration failed")
			->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
			->display();
		return FALSE;
	}
	ghInstance = hInstance;
	POINT upperLeftCorner;
	positionWindow(&upperLeftCorner, mainWindowWidth, mainWindowHeight);
	//centerWindow(&upperLeftCorner);
	mainWindowHwnd = CreateWindowEx
	(
		0,
		wc.lpszClassName,
		applicationOpMode == APPLICATION_OPERATION_MODE::SNAPSHOT ? L"SystemInfo (snapshot)" : L"SystemInfo",
		WS_SYSMENU | WS_OVERLAPPEDWINDOW,
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