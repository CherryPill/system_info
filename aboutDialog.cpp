#include "aboutDialog.h"
#include "utility.h"
BOOL CALLBACK aboutDlgProc(HWND dlgHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_NOTIFY: {
		switch (((LPNMHDR)lParam)->code) {
			case NM_CLICK:
			case NM_RETURN: {
				PNMLINK pNMLink = (PNMLINK)lParam;
				LITEM   item = pNMLink->item;
				if ((((LPNMHDR)lParam)->hwndFrom == GetDlgItem(dlgHandle, IDC_GITHUB)) && (item.iLink == 0)) {
					ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
				}
				break;
			}
		}
		break;
	}
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
	case WM_CLOSE: {
		EndDialog(dlgHandle, 0);
		return true;
	}
}
	return false;
}