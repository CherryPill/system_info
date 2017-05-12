#include "aboutDialog.h"
BOOL CALLBACK aboutDlgProc(HWND dlgHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		DWORD buffSize = 256;
		TCHAR inputBuff[256];
		ZeroMemory(inputBuff, sizeof(inputBuff));
		//set image to the static control

		//SendDlgItemMessage(dlgHandle, IDC_STATIC_ICON, STM_SETICON, (WPARAM)mainAppIcon, NULL);
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_NAME), appInfo);
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_VER), licenseInfo);
		SetWindowText(GetDlgItem(dlgHandle, IDC_PROGRAM_DESC), licenseInfo);
		SetWindowText(GetDlgItem(dlgHandle, IDC_GITHUB), licenseInfo);

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
}
	return false;
}