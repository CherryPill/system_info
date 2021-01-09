#include "../glb/colorVals.h"
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

std::vector<Control*> createControlButtons(SettingsWindow* sw) {
	map<TCHAR*, INT32> titleToIdMap = {
		{L"Save", IDC_SETTINGS_CONTROL_BTN_SAVE},
		{L"Cancel", IDC_SETTINGS_CONTROL_BTN_CANCEL},
		{L"Reset", IDC_SETTINGS_CONTROL_BTN_RESET},
	};
	DWORD buttonStyles = WS_CHILD | WS_VISIBLE | BS_CENTER;
	std::vector<Control*> btns;
	std::map<TCHAR*, INT32>::iterator mapIter;
	INT32 xOffset = sw->getWindowWidth() - (70*3) - 10;
	INT32 yOffset = sw->getEffectiveWindowHeight() + 20;
	for (mapIter = titleToIdMap.begin();
		mapIter != titleToIdMap.end(); mapIter++) {
		
		Control* sc = ControlBuilder::initBuilder()
			->withClassName(L"Button")
			->withWindowName(mapIter->first)
			->withControlId(mapIter->second)
			->withDimensions(60, 20)
			->withCoords(xOffset, yOffset)
			->withStyles(buttonStyles)
			->withParent(settingsDialogHwnd)
			->build().get();
		btns.push_back(sc);
		xOffset += 50 + 20;
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
	std::vector<Control*> controls;
	constexpr INT32 checkBoxSize = 6;
	TCHAR *settingsTab0SensorsBox[3] = {
		L"Show CPU usage",
		L"Show usable RAM",
		L"Show HDD temperature"
	};
	TCHAR* settingsTab0BehaviorBox[3] = {
		L"Screencap client area only",
		L"Remember last window position",
		L"Live uptime"
	};
	int yOffset = 30;
	int xOffset = 20;
	if (id == TAB_CONTENT_WRAPPER_OFFSET) {
		for (int t = 0; t < 3; t++, yOffset += 25) {
			std::unique_ptr<Control> sc = ControlBuilder::initBuilder()
				->withClassName(WC_BUTTON)
				->withWindowName(settingsTab0SensorsBox[t])
				->withStyles(WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX)
				->withCoords(xOffset, yOffset)
				->withDimensions(110, 25)
				->withParent(controlTabWrapperHandle)
				->withControlId(SETTINGS_TAB0_GROUP_SENSORS_IDS[t])
				->build();
	
			SendMessage(sc->getControlHandle(), BM_SETCHECK, checkBoxCheckedState.at(SETTINGS_TAB0_GROUP_SENSORS_IDS[t]), NULL);
			controls.push_back(sc.get());

			if (t == 2) {
				std::unique_ptr<SIZE> dimsForStatic(getAdjustedDimensionsForStaticCntrl(sc->getControlHandle(), settingsTab0SensorsBox[t]));
				resizeWindow(controlTabWrapperHandle, sc->getControlHandle(), dimsForStatic.get()->cx);

				std::unique_ptr<Control> uacIconCntrl =
					ControlBuilder::initBuilder()
					->withClassName(WC_STATIC)
					->withWindowName(settingsTab0SensorsBox[t])
					->withStyles(WS_CHILD | WS_VISIBLE | SS_ICON | SS_CENTER)
					->withCoords(dimsForStatic.get()->cx + 20, yOffset)
					->withDimensions(16, 16)
					->withParent(controlTabWrapperHandle)
					->withControlId(TAB_CONTENT_UAC_ICON_STATIC)
					->build();
						
				SendDlgItemMessage(controlTabWrapperHandle,
					TAB_CONTENT_UAC_ICON_STATIC,
					STM_SETICON,
					(WPARAM)uacIcon,
					NULL);
				controls.push_back(uacIconCntrl.get());
				yOffset += 16 + 10 + 5;

				
				std::unique_ptr<Control> tempUnitComboLabel = ControlBuilder::initBuilder()
					->withClassName(WC_STATIC)
					->withWindowName(L"Units:")
					->withCoords(xOffset + 50, yOffset)
					->withDimensions(50, 25)
					->withParent(controlTabWrapperHandle)
					->withStyles(WS_VISIBLE | WS_CHILD)
					->withControlId(TAB_CONTENT_COMBO_HDD_TEMP_UNIT)
					->build();
				controls.push_back(tempUnitComboLabel.get());




				
				std::unique_ptr<Control> tempUnitCombo = ControlBuilder::initBuilder()
					->withClassName(WC_COMBOBOX)
					->withCoords(xOffset + 80, yOffset)
					->withDimensions(80, 25)
					->withParent(controlTabWrapperHandle)
					->withStyles(CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE)
					->withControlId(TAB_CONTENT_COMBO_HDD_TEMP_UNIT)
					->build();

				fillComboBoxTemp(tempUnitCombo.get());

				controls.push_back(tempUnitCombo.get());
				yOffset += 10;

		}
		/*for (int t = 0; t < checkBoxSize; t++, yOffset += 25) {
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
				(HMENU)SETTINGS_WINDOW_CHKBOX_IDS[t],
				NULL,
				NULL);
			SendMessage(hwnd, BM_SETCHECK, checkBoxCheckedState.at(SETTINGS_WINDOW_CHKBOX_IDS[t]), NULL);
			sc->setControlHandle(hwnd);
			controls.push_back(sc);
			
			if (t == 1) {
				std::unique_ptr<SIZE> dimsForStatic(getAdjustedDimensionsForStaticCntrl(hwnd, checkBoxNames[t]));
				resizeWindow(controlTabWrapperHandle, hwnd, dimsForStatic.get()->cx); 
				SettingsControl* uacIconCntrl = new SettingsControl();
				HWND uacIconCntrlHwnd = CreateWindowEx(0,
					_T("Static"),
					NULL,
					WS_CHILD |
					WS_VISIBLE |
					SS_ICON | SS_CENTER,
					dimsForStatic.get()->cx+20, yOffset,
					16, 16,
					controlTabWrapperHandle,
					(HMENU)TAB_CONTENT_UAC_ICON_STATIC,
					NULL,
					NULL);
				SendDlgItemMessage(controlTabWrapperHandle,
					TAB_CONTENT_UAC_ICON_STATIC,
					STM_SETICON,
					(WPARAM)uacIcon,
					NULL);
				uacIconCntrl->setControlHandle(uacIconCntrlHwnd);
				controls.push_back(uacIconCntrl);
			}*/
		}
	
		std::unique_ptr<Control> gbControl =
			ControlBuilder::initBuilder()
			->withClassName(L"Button")
			->withWindowName(L"Sensors / Resource monitor")
			->withStyles(WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_CENTER)
			->withCoords(10, 10)
			->withDimensions(180, yOffset)
			->withParent(controlTabWrapperHandle)
			->build();
		controls.push_back(gbControl.get());

		yOffset = 30;
		xOffset = 210;
		for (int t = 0; t < 3; t++, yOffset += 25) {
			std::unique_ptr<Control> sc = ControlBuilder::initBuilder()
				->withClassName(WC_BUTTON)
				->withWindowName(settingsTab0BehaviorBox[t])
				->withStyles(WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX)
				->withCoords(xOffset, yOffset)
				->withDimensions(180, 25)
				->withParent(controlTabWrapperHandle)
				->withControlId(SETTINGS_TAB0_GROUP_BEHAVIOR_IDS[t])
				->build();



			SendMessage(sc->getControlHandle(), BM_SETCHECK, checkBoxCheckedState.at(SETTINGS_TAB0_GROUP_BEHAVIOR_IDS[t]), NULL);
			controls.push_back(sc.get());
		}
		yOffset += 5;
		
		std::unique_ptr<Control> ipAddrStaticCntrl = ControlBuilder::initBuilder()
			->withClassName(WC_STATIC)
			->withWindowName(L"IP Address information: ")
			->withCoords(xOffset, yOffset)
			->withDimensions(180, 25)
			->withParent(controlTabWrapperHandle)
			->withStyles(WS_VISIBLE | WS_CHILD)
			->withControlId(TAB_CONTENT_STATIC_IP_ADDR)
			->build();

		controls.push_back(ipAddrStaticCntrl.get());
		

	
		std::unique_ptr<Control> ipAddrBehaviorCombo = ControlBuilder::initBuilder()
			->withClassName(WC_COMBOBOX)
			->withCoords(xOffset + 110, yOffset)
			->withDimensions(120, 25)
			->withParent(controlTabWrapperHandle)
			->withStyles(CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE)
			->withControlId(TAB_CONTENT_COMBO_IP_ADDR_VALS)
			->build();

		fillComboBox(ipAddrBehaviorCombo.get());
		SendMessage(ipAddrBehaviorCombo.get()->getControlHandle(), CB_SETCURSEL, comboBoxState.at(TAB_CONTENT_COMBO_IP_ADDR_VALS), NULL);


		controls.push_back(ipAddrBehaviorCombo.get());


		std::unique_ptr<Control> _gbControl = 

			ControlBuilder::initBuilder()
			->withClassName(WC_BUTTON)
			->withWindowName(L"Program behavior")
			->withStyles(WS_VISIBLE | WS_CHILD | BS_GROUPBOX | BS_CENTER)
			->withCoords(200, 10)
			->withDimensions(250, yOffset += 25)
			->withParent(controlTabWrapperHandle)
			->build();
	
		controls.push_back(_gbControl.get());


		
		
		
	}
	else if (id == TAB_CONTENT_WRAPPER_OFFSET + 1) {

	std::unique_ptr<Control> htmlExportHeader = ControlBuilder::initBuilder()
		->withClassName(WC_STATIC)
		->withWindowName(L"HTML export theme:")
		->withStyles(WS_VISIBLE | WS_CHILD)
		->withCoords(10, 10)
		->withDimensions(100, 15)
		->withParent(controlTabWrapperHandle)
		->build();
		
		controls.push_back(htmlExportHeader.get());
		INT32 groupBoxOffset = createControlGroupBox(controlTabWrapperHandle, controls, 30);
		//to do: preview label
		//to do:button below tabwrapper
		std::unique_ptr<Control> htmlHeaderBlock =
			ControlBuilder::initBuilder()
			->withClassName(WC_STATIC)
			->withWindowName(L" Operating System")
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(10, groupBoxOffset)
			->withDimensions(200, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_ÑONTENT_HTML_EXPORT_HEADER)
			->build();

		controls.push_back(htmlHeaderBlock.get());

		std::unique_ptr<Control> htmlInfoBlock =
			ControlBuilder::initBuilder()
			->withClassName(WC_STATIC)
			->withWindowName((TCHAR*)sw->getExportInfoPreviewOsString().c_str())
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(10, groupBoxOffset + 20)
			->withDimensions(500, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_ÑONTENT_HTML_EXPORT_INFO)
			->build();

	
		controls.push_back(htmlInfoBlock.get());
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


INT32 createControlGroupBox(HWND hwnd, std::vector<Control*> controls, INT32 yOffset) {

	map<TCHAR*, std::vector<INT32>> titleToIdMap = {
	{L"Header", {TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR, TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR}},
	{L"Information", {TAB_ÑONTENT_HTML_EXPORT_INFO_BG_COLOR, TAB_ÑONTENT_HTML_EXPORT_INFO_FG_COLOR}}};
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

		
		std::unique_ptr<Control> htmlHeaderBlockBgColor =
			ControlBuilder::initBuilder()
			->withClassName(WC_STATIC)
			->withWindowName(
				convertColorReftoHexColorString(
					_tcscmp(mapIterator->first, L"Header") == 0 ?
					glbUserSettings->getHtmlExportHeaderBgColorRGB() :
					glbUserSettings->getHtmlExportInfoBgColorRGB()))
			->withStyles(colorChooserStaticControlStyles)
			->withCoords(xOffset * 2 + colorChooserBoxRightMargin, yOffset)
			->withDimensions(colorChooserBoxWidth, colorChooserBoxHeight)
			->withParent(hwnd)
			->withControlId(mapIterator->second.at(0))
			->build();

		
		controls.push_back(htmlHeaderBlockBgColor.get());

		std::unique_ptr<Control> htmlHeaderBlockFgColor =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(convertColorReftoHexColorString(
				_tcscmp(mapIterator->first, L"Header") == 0 ?
				glbUserSettings->getHtmlExportHeaderFgColorRGB() :
				glbUserSettings->getHtmlExportInfoFgColorRGB()
			))
			->withStyles(colorChooserStaticControlStyles)
			->withCoords(xOffset * 2 + colorChooserBoxWidth + colorChooserBoxRightMargin, yOffset)
			->withDimensions(colorChooserBoxWidth, colorChooserBoxHeight)
			->withParent(hwnd)
			->withControlId(mapIterator->second.at(1))
			->build();
	
		controls.push_back(htmlHeaderBlockBgColor.get());

		std::unique_ptr<Control> htmlExportSettingsHeaderThemeGroupBoxSC = 
			ControlBuilder::initBuilder()
			->withClassName(L"Button")
			->withWindowName(mapIterator->first)
			->withStyles(groupBoxStyles)
			->withCoords(xOffset * 2, yOffsetGroupBox)
			->withDimensions((colorChooserBoxWidth + colorChooserBoxMargin) * 2,
				(colorChooserBoxHeight + colorChooserBoxMargin * 2))
			->withParent(hwnd)
			->build();
		controls.push_back(htmlExportSettingsHeaderThemeGroupBoxSC.get());
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
		std::vector<Control*> controlBtns = createControlButtons(sw);
		sw->setControlButtons(controlBtns);
		sw->setTabs(tabs);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET] = SW_SHOW;
		sw->showTabs();
		EnumChildWindows(dialogWindow, (WNDENUMPROC)__SetFont, (LPARAM)helveticaFont); //setting the fonr
		break;
	}
	case WM_COMMAND: {
		WORD receivedCommand = LOWORD(wParam);
		WORD receivedHiWordCmd = HIWORD(wParam);
		switch (receivedCommand) {
			case IDC_SETTINGS_CONTROL_BTN_SAVE: {
				SavedUserSettingsHelper::readUISettingsState(sw->getTabs().at(0)->getTabContentWrapperHandle());
				DestroyWindow(dialogWindow);
				break;
			}
			case IDC_SETTINGS_CONTROL_BTN_CANCEL: {
				SendMessage(settingsDialogHwnd, WM_CLOSE, NULL, NULL);
				break;
			}
			case IDC_SETTINGS_CONTROL_BTN_RESET: {
				break;
			}
		}
		switch (receivedHiWordCmd) {
			case CBN_SELCHANGE: {
				//trap combobox selection change 
				IP_ADDR_BEHAVIOR currentlySelectedIndex = (IP_ADDR_BEHAVIOR)SendMessage(
					(HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if (currentlySelectedIndex == IP_ADDR_BEHAVIOR::SHOW) {
					GenericMessageOK()
						.withMessage(ipAddrShowWarnMsg)
						->withIcon(ControlManager::UI_MESS_RES_ICON::WARN)
						->display();
				}
				break;
			}
		}
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
		INT32 controlId = LOWORD(wParam);
		switch (controlId) {
			case TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR: {
				glbUserSettings->setHtmlExportHeaderBgColorRGB(initializeColorDlgBox(hwnd));
				updateWindow(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER);

				SetWindowText(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_BG_COLOR),
					convertColorReftoHexColorString(glbUserSettings->getHtmlExportHeaderBgColorRGB()));
				break;
			}
			case TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR: {
				glbUserSettings->setHtmlExportHeaderFgColorRGB(initializeColorDlgBox(hwnd));
				updateWindow(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER);
				SetWindowText(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR),
					convertColorReftoHexColorString(glbUserSettings->getHtmlExportHeaderFgColorRGB()));
				break;
			}
			case TAB_ÑONTENT_HTML_EXPORT_INFO_FG_COLOR: {
				glbUserSettings->setHtmlExportInfoFgColorRGB(initializeColorDlgBox(hwnd));
				updateWindow(hwnd, TAB_ÑONTENT_HTML_EXPORT_INFO);
				SetWindowText(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_INFO_FG_COLOR),
					convertColorReftoHexColorString(glbUserSettings->getHtmlExportInfoFgColorRGB()));
				break;
			}
			case TAB_ÑONTENT_HTML_EXPORT_INFO_BG_COLOR: {
				glbUserSettings->setHtmlExportInfoBgColorRGB(initializeColorDlgBox(hwnd));
				updateWindow(hwnd, TAB_ÑONTENT_HTML_EXPORT_INFO);
				SetWindowText(GetDlgItem(hwnd, TAB_ÑONTENT_HTML_EXPORT_INFO_BG_COLOR),
					convertColorReftoHexColorString(glbUserSettings->getHtmlExportInfoBgColorRGB()));
				break;
			}
			updateWindow(hwnd, controlId);
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
			
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderBgColorRGB());
			SetTextColor(hdcStatic, ColorUtil::pickAppropriateFgSysColor(glbUserSettings->getHtmlExportHeaderBgColorRGB()));
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderBgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_HEADER_FG_COLOR) {
			SetTextColor(hdcStatic, ColorUtil::pickAppropriateFgSysColor(glbUserSettings->getHtmlExportHeaderFgColorRGB()));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderFgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderFgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_INFO_BG_COLOR) {
			SetTextColor(hdcStatic, ColorUtil::pickAppropriateFgSysColor(glbUserSettings->getHtmlExportInfoBgColorRGB()));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportInfoBgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportInfoBgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_ÑONTENT_HTML_EXPORT_INFO_FG_COLOR) {
			SetTextColor(hdcStatic, ColorUtil::pickAppropriateFgSysColor(glbUserSettings->getHtmlExportInfoFgColorRGB()));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportInfoFgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportInfoFgColorRGB());
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
std::unordered_map<WPARAM, WPARAM> checkBoxCheckedState;
std::unordered_map<WPARAM, WPARAM> comboBoxState;

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

void fillComboBox(Control *control) {
	control->getControlHandle();
	unordered_map<IP_ADDR_BEHAVIOR, std::vector<TCHAR*>>::iterator mapIterator;
	for (mapIterator = ipAddrBehaviorDesc.begin();
		mapIterator != ipAddrBehaviorDesc.end();
		mapIterator++) {
		SendMessage(control->getControlHandle(), 
			(UINT)CB_ADDSTRING, 
			NULL, 
			(LPARAM)mapIterator->second.at(0));
	}
	SendMessage(control->getControlHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}
void fillComboBoxTemp(Control* control) {
	control->getControlHandle();
	unordered_map<TEMP_UNIT, std::vector<TCHAR*>>::iterator mapIterator;
	for (mapIterator = tempDesc.begin();
		mapIterator != tempDesc.end();
		mapIterator++) {
		SendMessage(control->getControlHandle(),
			(UINT)CB_ADDSTRING,
			NULL,
			(LPARAM)mapIterator->second.at(0));
	}
	SendMessage(control->getControlHandle(), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}
