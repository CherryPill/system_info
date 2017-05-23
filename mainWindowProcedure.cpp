#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <tchar.h>
#include "resource.h"
#include "SystemInfo.h"
#include "sysinfo.h"
#include "globalVars.h"
#include "appconst.h"
#include "itemIDs.h"
#include "iconLabelIDs.h"
#include "mainWindowProcedure.h"
#include "screenCapture.h"
#include "utility.h"
#include "saveSpecs.h"
#include "aboutDialog.h"
LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	SystemInfo *localMachine = localMachine->getCurrentInstance();
	switch (msg) {
		case WM_CREATE:{
			loadImages();
		    getSystemInformation(localMachine->getCurrentInstance());
			createHardwareInfoHolders(hwnd, localMachine->getCurrentInstance());
			populateInfoHolders(localMachine->getCurrentInstance(), hwnd);
			
			EnumChildWindows(hwnd, 
			(WNDENUMPROC)SetFont, 
			(LPARAM)GetStockObject(DEFAULT_GUI_FONT)); //setting the font
			RECT rc = {0};
			GetClientRect(hwnd, &rc);
			SCROLLINFO si = { 0 };
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL;
			si.nMin = 5;
			si.nMax = scrollFullPageHeight;
			si.nPage = (rc.bottom - rc.top);
			si.nPos = 0;
			si.nTrackPos = 0;
			SetScrollInfo(hwnd, SB_VERT, &si, true);
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_FILE_TAKESCREENSHOT: {
					takeScreenshot(hwnd);
					break;
				}
				case ID_FILE_SAVEASXML: {
					saveSpecs::saveAsXML(hwnd, localMachine->getCurrentInstance());
					break;
				}
				case ID_FILE_SAVEASTXT: {
					saveSpecs::saveAsText(hwnd, localMachine->getCurrentInstance());
					break;
				}
				case ID_FILE_SAVEASHTML: {
					saveSpecs::saveAsHTML(hwnd, localMachine->getCurrentInstance());
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
			}
		}
		case WM_CTLCOLORSTATIC: {
			HDC hdcStatic = (HDC)wParam;
			if ( GetDlgCtrlID((HWND)lParam) < BIOS_INFO) {
				SetTextColor(hdcStatic, RGB(125, 207, 246));
			}
			else {
				SetTextColor(hdcStatic, RGB(255, 255, 255));
			}
			SetBkColor(hdcStatic, RGB(81, 81, 81));
			return (LONG)grayBrush;
			break;
		}
		case WM_VSCROLL: {		
			auto action = LOWORD(wParam);
			HWND hScroll = (HWND)lParam;
			int pos = -1;
			if (action == SB_THUMBPOSITION || action == SB_THUMBTRACK) {
				pos = HIWORD(wParam);
			}
			else if (action == SB_LINEDOWN) {
				pos = scrollY + 30;
			}
			else if (action == SB_LINEUP) {
				pos = scrollY - 30;
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
			pt.y = pos - scrollY;
			auto hdc = GetDC(hwnd);
			LPtoDP(hdc, &pt, 1);
			ReleaseDC(hwnd, hdc);
			ScrollWindow(hwnd, 0, -pt.y, NULL, NULL);
			scrollY = pos;
			return 0;
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(WM_QUIT);
			return 0;
		}
		
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
BOOL CALLBACK SetFont(HWND child, LPARAM font) {
	SendMessage(child, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
	return TRUE;
}
void loadImages(void) {
	for (int x = 0;x<11;x++)
	{
		HICON newIcon = (HICON)LoadImage(ghInstance, 
		MAKEINTRESOURCE(ICON_IDS[x]), IMAGE_ICON, 16, 16, NULL);
		iconArr.push_back(newIcon);
	}
}

void createHardwareInfoHolders(HWND parent, SystemInfo *info) {
	UINT32 yStartOffSet = 20;
	int xStartOffSetLabel = 80;
	int xStartOffSetInformation = xStartOffSetLabel + 25 ;
	//labels + information as a table
	for (int x = 0, y = BIOS_INFO,z=BIOS_ICON_LABEL;x<BIOS_INFO;x++,y++,z++) {
		int sentinel = 0xf;
		//icon
		CreateWindowEx
			(
				0,
				L"Static",
				NULL,
				WS_VISIBLE |
				WS_CHILD |
				SS_ICON | SS_CENTER,
				xStartOffSetLabel-28,
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
		//info
		CreateWindowEx(
			0,
			L"Static",
			(L"Detecting..."+itemStrings[x]).c_str(),
			WS_VISIBLE | WS_CHILD |SS_LEFT | DS_SETFONT,
			xStartOffSetInformation,
			yStartOffSet+16,
			ITEM_INFO_WIDTH,
			ITEM_INFO_HEIGHT,
			parent,
			(HMENU)y,
			NULL,
			NULL
		);
		if (y >= GPU_INFO  && y < AUDIO_INFO)
		{
			UINT32 listSize = isAdjustRequired(y, info);
			//return rec structure
			if (listSize>2)
			{
				yStartOffSet = adjustItemHeight(parent, y, listSize);
				continue;
			}
		}
		yStartOffSet += (ITEM_HEIGHT + 10);
	}
	scrollFullPageHeight = yStartOffSet;
}
void populateInfoHolders(SystemInfo *currentMachineInfo, HWND mainWindowHwnd)
{
		SetWindowText(GetDlgItem(mainWindowHwnd, BIOS_INFO),
		currentMachineInfo->getBIOS().c_str());
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
						HARDWARE_VECTOR_TYPE::HARDWARE_VIDEO_ADAPTER, WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, MONITOR_INFO),
		formListString(currentMachineInfo,
			HARDWARE_VECTOR_TYPE::HARDWARE_DISPLAY, WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, STORAGE_INFO),
		formListString(currentMachineInfo,
			HARDWARE_VECTOR_TYPE::HARDWARE_STORAGE, WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, OPTICAL_INFO),
		formListString(currentMachineInfo,
			HARDWARE_VECTOR_TYPE::HARDWARE_CDROM, WRITE_OUT_TYPE::APP_WINDOW).c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, AUDIO_INFO),
		currentMachineInfo->getAudio().c_str());
	SetWindowText(GetDlgItem(mainWindowHwnd, UPTIME_INFO),
		currentMachineInfo->getUptime().c_str());
}
