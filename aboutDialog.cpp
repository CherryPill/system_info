#include "aboutDialog.h"
BOOL CALLBACK aboutDlgProc(HWND dlgHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		DWORD buffSize = 256;
		TCHAR inputBuff[256];
		ZeroMemory(inputBuff, sizeof(inputBuff));
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_NAME), (LPCWSTR)shippedSoftWare->getSoftwareName());
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_VER), (LPCWSTR)shippedSoftWare->getSoftwareVer());
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_DESC), (LPCWSTR)shippedSoftWare->getSoftwareDesc());
		SetWindowText(GetDlgItem(dlgHandle, IDC_GITHUB), (LPCWSTR)shippedSoftWare->getSoftwareRepo());
		SendMessage(GetDlgItem(dlgHandle, IDC_PROGRAM_NAME), WM_SETFONT, (WPARAM)appNameFontLarge, MAKELPARAM(true, 0));
		ZeroMemory(inputBuff, sizeof(inputBuff));
		return true;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_ABOUT_OK: {
			EndDialog(dlgHandle, 0);
			return true;
		}
	}

		break;
	}
	case WM_QUIT: {
		EndDialog(dlgHandle, 0);
		return true;
	}
}
	return false;
}