#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <tchar.h>
#include <process.h>
#include "resource.h"
#include "core/SystemInfo.h"
#include "core/sysinfo.h"
#include "glb/globalVars.h"
#include "const/appconst.h"
#include "const/itemIDs.h"
#include "const/iconLabelIDs.h"
#include "mainWindowProcedure.h"
#include "export/screenCapture.h"
#include "util/utility.h"
#include "export/saveSpecs.h"
#include "dialog/aboutDialog.h"
#include "dialog/scrUploadDialog.h"
#include "import/binImport.h"

int g_scrollY = 0;
LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	SystemInfo *localMachine = localMachine->getCurrentInstance();
	PAINTSTRUCT ps;
	HDC hdc;
	SCROLLINFO si;
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	switch (msg) {
		case WM_CREATE: {
			loadImages();
			if (PROGRAM_INSTANCE == 1) {
				importData(localMachine->getCurrentInstance());
			} else {
				fillSystemInformation(localMachine->getCurrentInstance());
				//test();
			}
			fillGUI(hwnd, localMachine, 0);
			toggleIpAddress(hwnd, NULL);
			return 0;
		}

		case WM_COMMAND: {
			WORD receivedCommand = LOWORD(wParam);
			switch (receivedCommand) {
				case ID_FILE_TAKESCREENSHOT_SAVE_LOCALLY: {
					takeScreenshot(hwnd, SCR_SAVETYPE::LOCAL);
					break;
				}
				case ID_FILE_TAKESCREENSHOT_UPLOAD: {
					takeScreenshot(hwnd, SCR_SAVETYPE::INTERNET);
					DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG_SCRUPLOAD), hwnd, (DLGPROC)scrDlgProc);

					break;
				}
				case ID_EXPORT_XML:
				case ID_EXPORT_TXT:
				case ID_EXPORT_HTML: {
					TCHAR *dateTime = new TCHAR[256];
					//export xml
					getCurrentDateTimeVerbose(dateTime);
					wstring dateTimeConv(dateTime);
					localMachine->
						getCurrentInstance()->
						setSnapshotGenDateTime(dateTimeConv);
					delete dateTime;
					RESULT_STRUCT resStruct = {};

					saveSpecs::save(receivedCommand, &resStruct, hwnd, localMachine->getCurrentInstance());
					if (resStruct.result) {
						displayExportMessage(UI_MESS_RES::SUCCESS, getUIMessByCommand(receivedCommand));
						if (displayPromptForAction(actionPromptText[0]) == IDYES) {
							if (openDefAppForExpData(receivedCommand, &resStruct) != TRUE) {
								displayMessageGeneric(UI_MESS_RES::FAILURE, L"Unable to open exported data");
							}
						}
						break;
					}
					break;
				}
				case ID_IMPORT_FROMXML: {
					importAsXML(hwnd);
					break;
				}
				case ID_ABOUT: {
					DialogBox(ghInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)aboutDlgProc);
					break;
				}
				case ID_FILE_EXIT: {
					PostQuitMessage(WM_QUIT);
					break;
				}
				case AUX_IP_TOGGLE: {
					toggleIpAddress(hwnd, localMachine);
					break;
				}
			}
		}
						 //to do put hardcoded color values in external constants
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam;
			if (GetDlgCtrlID((HWND)lParam) < BIOS_INFO) {
				if (GetDlgCtrlID((HWND)lParam) == SNAPSHOT_LABEL) {
					SetTextColor(hdcStatic, RGB(0, 255, 0));
				} else {
					SetTextColor(hdcStatic, RGB(125, 207, 246));
				}
			} else {
				if (GetDlgCtrlID((HWND)lParam) == AUX_IP_TOGGLE) {
					SetTextColor(hdcStatic, RGB(255, 0, 0));

				}
				SetTextColor(hdcStatic, RGB(255, 255, 255));
			}
			SetBkColor(hdcStatic, RGB(81, 81, 81));
			return (LONG)grayBrush;
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(WM_QUIT);
			return 0;
		}
		case WM_PAINT: {
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}
		case WM_LBUTTONDOWN: {
			SCROLLINFO si = { 0 };
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			si.nPos = 0;
			si.nTrackPos = 0;
			GetScrollInfo(hwnd, SB_VERT, &si);

			break;
		}
		case WM_VSCROLL: {
			auto action = LOWORD(wParam);
			HWND hScroll = (HWND)lParam;
			int pos = -1;
			if (action == SB_THUMBPOSITION || action == SB_THUMBTRACK) {
				pos = HIWORD(wParam);
			} else if (action == SB_LINEDOWN) {
				pos = g_scrollY + 30;
			} else if (action == SB_LINEUP) {
				pos = g_scrollY - 30;
			}
			if (pos == -1)
				break;
			WCHAR buf[20];
			SCROLLINFO si = { 0 };
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			si.nPos = pos;
			si.nTrackPos = 0;
			SetScrollInfo(hwnd, SB_VERT, &si, true);
			GetScrollInfo(hwnd, SB_VERT, &si);
			pos = si.nPos;
			POINT pt;
			pt.x = 0;
			pt.y = pos - g_scrollY;
			auto hdc = GetDC(hwnd);
			LPtoDP(hdc, &pt, 1);
			ReleaseDC(hwnd, hdc);
			ScrollWindow(hwnd, 0, -pt.y, NULL, NULL);
			g_scrollY = pos;
			return 0;
		}
		case WM_SIZE: {
			int yClient = HIWORD(lParam);
			int xClient = LOWORD(lParam);
			if (wParam != SIZE_RESTORED && wParam != SIZE_MAXIMIZED)
				break;

			int x = GetScrollPos(hwnd, SB_VERT);

			SCROLLINFO si = {};
			si.cbSize = sizeof(SCROLLINFO);

			const int bar[] = { SB_HORZ, SB_VERT };
			const int page[] = { LOWORD(lParam), HIWORD(lParam) };

			for (size_t i = 0; i < ARRAYSIZE(bar); ++i) {
				si.fMask = SIF_PAGE;
				si.nPage = page[i];
				SetScrollInfo(hwnd, bar[i], &si, TRUE);

				si.fMask = SIF_RANGE | SIF_POS;
				GetScrollInfo(hwnd, bar[i], &si);

				const int maxScrollPos = si.nMax - (page[i] - 1);

				const bool scrollRequired =
					(si.nPos != si.nMin && si.nPos == maxScrollPos) ||
					(wParam == SIZE_MAXIMIZED);

				if (scrollRequired) {
					scrollClientWindow(hwnd, bar[i], si.nPos);
				}
			}
			break;
		}
		case WM_MOUSEWHEEL: {
			if (((short)HIWORD(wParam)) / 120 > 0)
				PostMessage(hwnd, WM_VSCROLL, SB_LINEUP, (LPARAM)0);
			if (((short)HIWORD(wParam)) / 120 < 0)
				PostMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)0);
			return (0);
			break;
		}
		case WM_GETMINMAXINFO: {
			lpMMI->ptMinTrackSize.x = mainWindowWidth;
			lpMMI->ptMinTrackSize.y = mainWindowHeight;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void fillGUI(HWND hwnd, SystemInfo *localMachine, int indexOffset) {
	createHardwareInfoHolders(hwnd, localMachine->getCurrentInstance(), indexOffset);
	populateInfoHolders(localMachine->getCurrentInstance(), hwnd);

	RECT rc = { 0 };
	GetClientRect(hwnd, &rc);
	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = scrollFullPageHeight;
	si.nPage = (rc.bottom - rc.top);
	si.nPos = 0;
	si.nTrackPos = 0;
	SetScrollInfo(hwnd, SB_VERT, &si, true);

	EnumChildWindows(hwnd,
		(WNDENUMPROC)SetFont,
					 (LPARAM)GetStockObject(DEFAULT_GUI_FONT)); //setting the font
}

BOOL CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	SendMessage(GetDlgItem(GetParent(child), AUX_IP_TOGGLE),
				WM_SETFONT,
				(WPARAM)ipToggleBtnFont, MAKELPARAM(true, 0));
	return TRUE;
}

void loadImages(void) {
	for (int x = 0; x < totalItemsCount; x++) {
		HICON newIcon = (HICON)LoadImage(ghInstance,
										 MAKEINTRESOURCE(ICON_IDS[x]),
										 IMAGE_ICON,
										 ITEM_ICON_SIZE,
										 ITEM_ICON_SIZE,
										 NULL);
		iconArr.push_back(newIcon);
	}
}

void createHardwareInfoHolders(HWND parent, SystemInfo *info, int offsetIndex) {
	if (PROGRAM_INSTANCE == 1) {
		CreateWindowEx
		(
			0,
			L"Static",
			(L"Snapshot as of " + info->getSnapshotGenDateTime()).c_str(),
			WS_VISIBLE |
			WS_CHILD |
			DS_SETFONT | SS_CENTER,
			mainWindowWidth / 2 - 200,
			0,
			400,
			ITEM_HEIGHT,
			parent,
			(HMENU)SNAPSHOT_LABEL,
			NULL,
			NULL
		);
	}
	UINT32 yStartOffSet = 20;
	int xStartOffSetLabel = 80;
	int xStartOffSetInformation = xStartOffSetLabel + 25;
	//labels + information as a table
	for (int x = offsetIndex, y = offsetIndex + INFO_ID_OFFSET, z = offsetIndex + ICON_ID_OFFSET;
		 x < BIOS_INFO;
		 x++, y++, z++) {
		//icon
		CreateWindowEx
		(
			0,
			L"Static",
			NULL,
			WS_VISIBLE |
			WS_CHILD |
			SS_ICON | SS_CENTER,
			xStartOffSetLabel - 28,
			yStartOffSet,
			ITEM_WIDTH,
			ITEM_HEIGHT,
			parent,
			(HMENU)z,
			NULL,
			NULL
		);
		SendDlgItemMessage(parent, z,
						   STM_SETICON,
						   (WPARAM)iconArr.at(x),
						   NULL);

		//text
		CreateWindowEx
		(
			0,
			L"Static",
			(LPCTSTR)itemStrings[x].c_str(),
			WS_VISIBLE |
			WS_CHILD |
			DS_SETFONT | SS_LEFT,
			xStartOffSetLabel,
			yStartOffSet,
			ITEM_WIDTH,
			ITEM_HEIGHT,
			parent,
			(HMENU)x,
			NULL,
			NULL
		);
		if (x == 9) {
			createIPToggleControl(parent, xStartOffSetLabel + 140, yStartOffSet);
		}
		//info
		CreateWindowEx(
			0,
			L"Static",
			(L"Detecting..." + itemStrings[x]).c_str(),
			WS_VISIBLE | WS_CHILD | SS_LEFT | DS_SETFONT,
			xStartOffSetInformation,
			yStartOffSet + 16,
			ITEM_INFO_WIDTH,
			getInfoBoxItemCount(y, info) * ITEM_INFO_INITIAL_HEIGTH,
			parent,
			(HMENU)y,
			NULL,
			NULL
		);
		if (y >= GPU_INFO  && y < AUDIO_INFO) {
			UINT32 listSize = getInfoBoxItemCount(y, info);
			//return rec structure
			if (listSize >= 2) {
				yStartOffSet = adjustItemHeight(parent, y, listSize);
				continue;
			}
		}
		yStartOffSet += (ITEM_HEIGHT + 2);
	}
	scrollFullPageHeight = yStartOffSet;

}

void populateInfoHolders(SystemInfo *currentMachineInfo, HWND mainWindowHwnd) {
	TCHAR biosInfo[256] = { 0 };
	_tcscat(biosInfo, currentMachineInfo->getBIOS().c_str());
	_tcscat(biosInfo, currentMachineInfo->getComputerType().c_str());

	SetWindowText(GetDlgItem(mainWindowHwnd, BIOS_INFO),
				  biosInfo);
	SetWindowText(GetDlgItem(mainWindowHwnd, OS_INFO),
				  currentMachineInfo->getOS().c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, CPU_INFO),
				  currentMachineInfo->getCPU().c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, MB_INFO),
				  currentMachineInfo->getMB().c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, RAM_INFO),
				  currentMachineInfo->getRAM().c_str());

	SetWindowText(GetDlgItem(mainWindowHwnd, GPU_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_VIDEO_ADAPTER,
								 WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, MONITOR_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_DISPLAY,
								 WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, STORAGE_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_STORAGE,
								 WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, OPTICAL_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_CDROM,
								 WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, NETWORK_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_NETWORK,
								 WRITE_OUT_TYPE::APP_WINDOW).c_str());
	if (currentMachineInfo->getNetworkAdaptersText().back().find(L"Unable") == string::npos) {
		//createIPToggleControl(GetDlgItem(mainWindowHwnd, NETWORK_INFO));
	}
	SetWindowText(GetDlgItem(mainWindowHwnd, AUDIO_INFO),
				  currentMachineInfo->getAudio().c_str());
	if (!PROGRAM_INSTANCE) {
		DWORD uptimeThreadId;
		HANDLE threadHandle = (HANDLE)_beginthreadex(0, 0, updateUptime, 0, 0, 0);
	} else {
		SetWindowText(GetDlgItem(mainWindowHwnd, UPTIME_INFO),
					  currentMachineInfo->getUptime().c_str());
	}
}

unsigned int __stdcall updateUptime(void *t) {
	static TCHAR timeBuff[256];
	while (true) {
		ZeroMemory(timeBuff, sizeof(timeBuff));
		calculateTimeAndFormat(timeBuff);
		BOOL res = SetWindowText(GetDlgItem(mainWindowHwnd, UPTIME_INFO), timeBuff);
		Sleep(500);
	}
	_endthreadex(0);
}

void createIPToggleControl(HWND parent, int xOff, int yOff) {
	CreateWindowEx(
		0,
		L"Button",
		ipToggleText[0].c_str(),
		WS_VISIBLE | WS_CHILD | WS_TABSTOP,
		xOff,
		yOff,
		64,
		15,
		parent,
		(HMENU)AUX_IP_TOGGLE,
		NULL,
		NULL
	);
}

void toggleIpAddress(HWND mainWindow, SystemInfo *info) {
	static bool showStatus = 0;
	static wstring savedIP;
	SetWindowText(GetDlgItem(mainWindow, AUX_IP_TOGGLE), ipToggleText[showStatus].c_str());
	if (info != NULL) {

		if (!showStatus) {
			info->getNetworkAdaptersTextRef().back().assign(savedIP);
		} else {
			savedIP.assign(info->getNetworkAdaptersText().back());
			//hide ip
			info->getNetworkAdaptersTextRef().back().clear();
			info->getNetworkAdaptersTextRef().back().assign(L"Connected to the Internet: (IP hidden by user)");

		}
		trimWhiteSpace(info->getNetworkAdaptersTextRef().back());
		updateNetworkAdaptersView(info);
	}
	showStatus = !showStatus;
}

void updateNetworkAdaptersView(SystemInfo *currentMachineInfo) {
	SetWindowText(GetDlgItem(mainWindowHwnd, NETWORK_INFO),
				  formListString(currentMachineInfo,
								 OS_INFO_TYPES::HARDWARE_NETWORK, WRITE_OUT_TYPE::APP_WINDOW).c_str());
}

void scrollClientWindow(HWND hwnd, int bar, int pos) {
	static int s_prevx = 1;
	static int s_prevy = 1;

	int cx = 0;
	int cy = 0;

	int& delta = (bar == SB_HORZ ? cx : cy);
	int& prev = (bar == SB_HORZ ? s_prevx : s_prevy);

	delta = prev - pos;
	prev = pos;

	if (cx || cy) {
		ScrollWindow(hwnd, cx, cy, NULL, NULL);
	}
}
