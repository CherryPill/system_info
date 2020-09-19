#include "../glb/globalVars.h"
#include "../const/itemIDs.h"
#include "settings.h"
#include "../util/utility.h"
#include "../mainWindowProcedure.h"
void registerSettingsDialogClass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = _T("Settings");
	wc.hInstance = ghInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = (WNDPROC)settingsDialogProcedure;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);
}

void registerTabContentWrapperWindowClass() {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = _T("TabContentWrapper");
	wc.hInstance = ghInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = (WNDPROC)tabWrapperProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);
}

std::vector<SettingsTab*> createAndFillTabs(SettingsWindow* sw) {

	const WPARAM headerCount = 3;
	TCITEM tabData;
	TCHAR* tabHeaderTxt[headerCount] = {
			_T("Main"),
			_T("Display"),
			_T("Miscellaneous")
	};

	dialogTabControl = CreateWindowEx(
		0,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0,
		0,
		440,
		500,
		settingsDialogHwnd,
		(HMENU)IDC_SETTINGS_TABAREA,
		NULL,
		NULL
	);
	std::vector<SettingsTab*> tabs;
	RECT tabHeaderRect;
	GetWindowRect(dialogTabControl, &tabHeaderRect);
	SendMessage(dialogTabControl, TCM_GETITEMRECT, NULL, (LPARAM)& tabHeaderRect);
	INT32 yOffset = tabHeaderRect.bottom - tabHeaderRect.top;
	for (WPARAM tabNum = 0; tabNum < headerCount - 1; tabNum++) {
		tabData.mask = TCIF_TEXT;
		tabData.pszText = tabHeaderTxt[tabNum];
		SendMessage(dialogTabControl, TCM_INSERTITEM, tabNum, (LPARAM)& tabData);
		SettingsTab* st = new SettingsTab();
		HWND contentWrapperHandler = createGenericContainer(
			TAB_CONTENT_WRAPPER_OFFSET + tabNum,
			yOffset,
			sw->getWindowWidth(),
			sw->getWindowHeight(),
			settingsDialogHwnd);
		st->setTabContentWrapperHandle(contentWrapperHandler);
		st->setTabBlocks(createControlsForTab(contentWrapperHandler, TAB_CONTENT_WRAPPER_OFFSET + tabNum));
		tabs.push_back(st);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET + tabNum] = SW_HIDE;
	}
	return tabs;
}

std::vector<SettingsBlock*> createControlsForTab(
	HWND controlTabWrapperHandle, 
	INT32 id) {
	std::vector<SettingsBlock*> settingsBlocks;
	SettingsBlock* sb = new SettingsBlock();
	std::vector<SettingsControl*> controls;
	TCHAR* checkBoxNames[3] = {
		L"Show CPU usage",
		L"Screencap client area only",
		L"Remember last window position"
	};
	//if (id == TAB_CONTENT_WRAPPER_OFFSET) {
		//for (int t = 0, yOffset = 0; t < 3; t++, yOffset += 50) {
			SettingsControl* sc = new SettingsControl();
			HWND hwnd = CreateWindowEx(0,
				_T("Button"),
				_T("Test"),
				WS_CHILD |
				WS_VISIBLE |
				BS_AUTOCHECKBOX,
				0, 0,
				50, 50,
				controlTabWrapperHandle,
				(HMENU)id + 1000 + 0,
				NULL,
				NULL);
			sc->setControlHandle(hwnd);
			controls.push_back(sc);
		//}
	//}
	sb->setControls(controls);
	settingsBlocks.push_back(sb);
	return settingsBlocks;
}

HWND createGenericContainer(INT32 id, INT32 yOffset, INT32 w, INT32 h, HWND parent) {
	return ControlManager::appCreateControl(
		_T("TabContentWrapper"),
		0,
		yOffset,
		w,
		h,
		WS_CHILD,
		settingsDialogHwnd,
		id,
		NULL,
		NULL,
		ghInstance,
		NULL
	);
}

HWND createTabContentWrapper(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent) {
	return createGenericContainer(id, w, h, yOffset, parent);
}

LRESULT CALLBACK settingsDialogProcedure(HWND dialogWindow, UINT message,
	WPARAM wParam, LPARAM lParam) {
	settingsDialogHwnd = dialogWindow;
	switch (message) {
	case WM_NOTIFY: {
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE: {
			handleTabSelectionChange(TabCtrl_GetCurSel(dialogTabControl));
			break;
		}
		}
		break;
	}
	case WM_CREATE: {
		std::vector<SettingsTab*> tabs = createAndFillTabs(sw);
		sw->setTabs(tabs);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET] = SW_SHOW;
		EnumChildWindows(dialogWindow, (WNDENUMPROC)__SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT)); //setting the fonr
		break;
	}
	case WM_COMMAND: {

		break;
	}
	case WM_CLOSE: {
		delete sw;
		break;
	}
	case WM_KEYDOWN: {
		break;
	}

	default: {
		break;
	}
	}
	return (DefWindowProcW(dialogWindow, message, wParam, lParam));
}


LRESULT CALLBACK tabWrapperProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		break;
	}
	}
	return (DefWindowProcW(hwnd, message, wParam, lParam));
}

void handleTabSelectionChange(INT32 selectedTabIndex) {
	try {
		if (selectedTabIndex < 0) {
			throw _T("Something went terribly wrong");
		}
		else {
			tabClickState[TAB_CONTENT_WRAPPER_OFFSET + selectedTabIndex] = TRUE;
			sw->showTabs();
			for (int c = 0; c < 2; c++) {
				tabClickState[TAB_CONTENT_WRAPPER_OFFSET + c] = FALSE;
			}
		}
	}
	catch (TCHAR * e) {
		MessageBox(NULL, e, _T("Runtime error"), MB_OK | MB_ICONERROR);
	}
}

bool CALLBACK __SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
	return true;
}
std::unordered_map<WPARAM, INT32> tabClickState;

COLORREF initializeColorDlgBox(HWND hwnd) {

	CHOOSECOLOR cc;
	COLORREF chosenColor;
	static COLORREF acrCustClr[16]; // array of custom colors 
						 // owner window
	HBRUSH hbrush;                  // brush handle
	static DWORD rgbCurrent;        // initial color selection

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		hbrush = CreateSolidBrush(cc.rgbResult);
		rgbCurrent = cc.rgbResult;
	}
	chosenColor = cc.rgbResult;
	return chosenColor;
}
