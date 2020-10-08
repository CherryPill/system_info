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

std::vector<SettingsControl*> createControlButtons(SettingsWindow* sw) {
	map<TCHAR*, INT32> titleToIdMap = {
		{L"Save", IDC_SETTINGS_CONTROL_BTN_SAVE},
		{L"Cancel", IDC_SETTINGS_CONTROL_BTN_CANCEL},
		{L"Reset", IDC_SETTINGS_CONTROL_BTN_RESET},
	};
	DWORD buttonStyles = ;
	std::vector<SettingsControl*> btns;
	std::map<TCHAR*, INT32>::iterator mapIter;
	for (mapIter = titleToIdMap.begin(); 
		mapIter != titleToIdMap.end(); mapIter++) {
		SettingsControl* sc = new SettingsControl();

		ControlBuilder::initBuilder()
			->withClassName(L"Button")
			->withControlId()
			->withCoords()
			->withParent();

		sc->setControlHandle();
	}

	return btns;
}

std::vector<SettingsTab*> createAndFillTabs(SettingsWindow* sw) {

	constexpr WPARAM headerCount = 3;
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
		sw->getWindowWidth(),
		sw->getWindowHeight(),
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
	for (WPARAM tabNum = 0; tabNum < headerCount; tabNum++) {
		tabData.mask = TCIF_TEXT;
		tabData.pszText = tabHeaderTxt[tabNum];
		SendMessage(dialogTabControl, TCM_INSERTITEM, tabNum, (LPARAM)& tabData);
		SettingsTab* st = new SettingsTab();
		HWND contentWrapperHandler = createGenericContainer(
			TAB_CONTENT_WRAPPER_OFFSET + tabNum,
			yOffset,
			sw->getWindowWidth(),
			sw->getEffectiveWindowHeight(),
			settingsDialogHwnd);
		tabControlWrapperProc = (WNDPROC)SetWindowLongPtr(contentWrapperHandler, GWL_WNDPROC, (LONG_PTR)& tabWrapperProc);

		st->setTabContentWrapperHandle(contentWrapperHandler);
		st->setTabBlocks(createControlsForTab(contentWrapperHandler, TAB_CONTENT_WRAPPER_OFFSET + tabNum, sw));
		tabs.push_back(st);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET + tabNum] = SW_HIDE;
	}
	return tabs;
}

std::vector<SettingsBlock*> createControlsForTab(
	HWND controlTabWrapperHandle,
	INT32 id, SettingsWindow* sw) {
	std::vector<SettingsBlock*> settingsBlocks;
	SettingsBlock* sb = new SettingsBlock();
	std::vector<SettingsControl*> controls;
	constexpr INT32 checkBoxSize = 4;
	TCHAR* checkBoxNames[checkBoxSize] = {
		L"Show CPU usage",
		L"Show Hard disks temperature",
		L"Screencap client area only",
		L"Remember last window position"
	};
	if (id == TAB_CONTENT_WRAPPER_OFFSET) {
		for (int t = 0, yOffset = 15; t < checkBoxSize; t++, yOffset += 25) {
			SettingsControl* sc = new SettingsControl();
			HWND hwnd = CreateWindowEx(0,
				_T("Button"),
				checkBoxNames[t],
				WS_CHILD |
				WS_VISIBLE |
				BS_AUTOCHECKBOX,
				15, yOffset,
				200, 25,
				controlTabWrapperHandle,
				(HMENU)id + 10 + t,
				NULL,
				NULL);
			sc->setControlHandle(hwnd);
			controls.push_back(sc);
		}
	}
	else if (id == TAB_CONTENT_WRAPPER_OFFSET + 1) {

		SettingsControl* htmlExportHeader = new SettingsControl();
		HWND htmlExportSettingsStaticHeader = ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(L"HTML export theme:")
			->withStyles(WS_VISIBLE |
				WS_CHILD)
			->withCoords(10, 10)
			->withDimensions(100, 15)
			->withParent(controlTabWrapperHandle)
			->build();
		htmlExportHeader->setControlHandle(htmlExportSettingsStaticHeader);
		controls.push_back(htmlExportHeader);
		INT32 groupBoxOffset = createControlGroupBox(controlTabWrapperHandle, controls, 30);
		//to do: preview label
		//to do:button below tabwrapper
		SettingsControl * htmlHeaderBlock = new SettingsControl();
		HWND htmlHeaderBlockStaticHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(L" Operating System")
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(10, groupBoxOffset)
			->withDimensions(200, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_ÑONTENT_HTML_EXPORT_HEADER)
			->build();

		htmlHeaderBlock->setControlHandle(htmlHeaderBlockStaticHwnd);
		controls.push_back(htmlHeaderBlock);

		SettingsControl * htmlInfoBlock = new SettingsControl();
		HWND htmlInfoBlockStaticHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName((TCHAR*)sw->getExportInfoPreviewOsString().c_str())
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(10, groupBoxOffset + 20)
			->withDimensions(500, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_ÑONTENT_HTML_EXPORT_INFO)
			->build();

		htmlHeaderBlock->setControlHandle(htmlInfoBlockStaticHwnd);
		controls.push_back(htmlInfoBlock);
	}
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
		parent,
		id,
		NULL,
		NULL,
		ghInstance,
		NULL
	);
}


INT32 createControlGroupBox(HWND hwnd, std::vector<SettingsControl*> controls, INT32 yOffset) {

	map<TCHAR*, std::vector<INT32>> titleToIdMap = {
	{L"Header", {TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR, TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR}},
	{L"Information", {TAB_ÑONTENT_HTML_EXPORT_INFO_BG_COLOR, TAB_ÑONTENT_HTML_EXPORT_INFO_FG_COLOR}},
	};
	INT32 xOffset = 10;
	INT32 colorChooserBoxWidth = 60;
	INT32 colorChooserBoxHeight = 40;
	INT32 colorChooserBoxRightMargin = 10;
	INT32 colorChooserBoxMargin = 20;
	INT32 colorGroupBoxBottomMargin = 30;
	INT32 singleBlockWidth = 0;
	INT32 groupBoxWidth = 0;
	DWORD colorChooserStaticControlStyles = WS_VISIBLE | WS_CHILD | SS_SUNKEN | SS_CENTERIMAGE | SS_CENTER | SS_NOTIFY;
	DWORD groupBoxStyles = WS_VISIBLE | WS_CHILD | BS_GROUPBOX;
	INT32 yOffsetGroupBox = yOffset;
	map<TCHAR*, std::vector<INT32>>::iterator mapIterator;
	for (mapIterator = titleToIdMap.begin(); mapIterator != titleToIdMap.end(); mapIterator++) {

		yOffset += 20;

		SettingsControl* htmlExportSettingsHeaderThemeGroupBoxSC = new SettingsControl();
		SettingsControl* htmlHeaderBlockBgColor = new SettingsControl();
		HWND htmlHeaderBlockBgColorHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(
				convertColorReftoHexColorString(
					glbUserSettings->getHtmlExportHeaderBgColorRGB()))
			->withStyles(colorChooserStaticControlStyles)
			->withCoords(xOffset * 2 + colorChooserBoxRightMargin, yOffset)
			->withDimensions(colorChooserBoxWidth, colorChooserBoxHeight)
			->withParent(hwnd)
			->withControlId(mapIterator->second.at(0))
			->build();

		htmlHeaderBlockBgColor->setControlHandle(htmlHeaderBlockBgColorHwnd);
		controls.push_back(htmlHeaderBlockBgColor);

		SettingsControl* htmlHeaderBlockFgColor = new SettingsControl();
		HWND htmlHeaderBlockFgColorHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(convertColorReftoHexColorString(
				glbUserSettings->getHtmlExportHeaderFgColorRGB()))
			->withStyles(colorChooserStaticControlStyles)
			->withCoords(xOffset * 2 + colorChooserBoxWidth + colorChooserBoxRightMargin, yOffset)
			->withDimensions(colorChooserBoxWidth, colorChooserBoxHeight)
			->withParent(hwnd)
			->withControlId(mapIterator->second.at(1))
			->build();
		htmlHeaderBlockBgColor->setControlHandle(htmlHeaderBlockFgColorHwnd);
		controls.push_back(htmlHeaderBlockBgColor);

		HWND htmlExportSettingsHeaderThemeGroupBox =
			ControlBuilder::initBuilder()
			->withClassName(L"Button")
			->withWindowName(mapIterator->first)
			->withStyles(groupBoxStyles)
			->withCoords(xOffset * 2, yOffsetGroupBox)
			->withDimensions((colorChooserBoxWidth + colorChooserBoxMargin) * 2,
			(colorChooserBoxHeight + colorChooserBoxMargin * 2))
			->withParent(hwnd)
			->build();
		htmlExportSettingsHeaderThemeGroupBoxSC->setControlHandle(htmlExportSettingsHeaderThemeGroupBox);
		controls.push_back(htmlExportSettingsHeaderThemeGroupBoxSC);
		xOffset += (colorChooserBoxWidth + 15 * 2);
		yOffset -= 20;
	}
	return yOffsetGroupBox + (colorChooserBoxHeight + colorChooserBoxMargin * 3);
	//yOffset += (colorChooserBoxHeight + colorChooserBoxMargin);
	//return yOffset;

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
		std::vector<SettingsControl> controlBtns = createControlButtons(sw);
		sw->setTabs(tabs);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET] = SW_SHOW;
		sw->showTabs();
		EnumChildWindows(dialogWindow, (WNDENUMPROC)__SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT)); //setting the fonr
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
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR: {

			glbUserSettings->setHtmlExportHeaderBgColorRGB(initializeColorDlgBox(hwnd));
			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR), SW_SHOW);

			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER), SW_SHOW);

			SetWindowText(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR),
				convertColorReftoHexColorString(glbUserSettings->getHtmlExportHeaderBgColorRGB()));
			break;
		}
		case TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR: {
			glbUserSettings->setHtmlExportHeaderFgColorRGB(initializeColorDlgBox(hwnd));
			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR), SW_SHOW);
			break;
		}
		}
		break;
	}
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_HEADER) {
			SetTextColor(hdcStatic, glbUserSettings->getHtmlExportHeaderFgColorRGB());
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderBgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderBgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_INFO) {
			SetTextColor(hdcStatic, glbUserSettings->getHtmlExportInfoFgColorRGB());
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportInfoBgColorRGB());
			return (long)CreateSolidBrush(RGB(255, 255, 255));
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR) {
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderBgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderBgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR) {
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderFgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderFgColorRGB());
		}

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
			for (int c = 0; c < 3; c++) {
				tabClickState[TAB_CONTENT_WRAPPER_OFFSET + c] = FALSE;
			}
			tabClickState[TAB_CONTENT_WRAPPER_OFFSET + selectedTabIndex] = TRUE;
			sw->showTabs();
		}
	}
	catch (TCHAR * e) {
		MessageBox(NULL, e, _T("Runtime error"), MB_OK | MB_ICONERROR);
	}
}

bool CALLBACK __SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
	SendMessage(GetDlgItem(GetParent(child), TAB_ÑONTENT_HTML_EXPORT_HEADER),
		WM_SETFONT,
		(WPARAM)helveticaFont, MAKELPARAM(true, 0));
	SendMessage(GetDlgItem(GetParent(child), TAB_ÑONTENT_HTML_EXPORT_INFO),
		WM_SETFONT,
		(WPARAM)helveticaFont, MAKELPARAM(true, 0));
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
