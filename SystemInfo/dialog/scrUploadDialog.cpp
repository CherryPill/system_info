#include "scrUploadDialog.h"
#include "../util/utility.h"
#include "../glb/globalVars.h"
UPLOAD_SRC_LINK_DATA uploadSrcLinkData = {};
BOOL CALLBACK scrDlgProc(HWND dlgHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			SendMessage(GetDlgItem(dlgHandle, IDC_PROGRAM_NAME), WM_SETFONT, (WPARAM)appNameFontLarge, MAKELPARAM(true, 0));
			SetWindowText(GetDlgItem(dlgHandle, IDC_EDIT_SCRCOPY), (LPCWSTR)uploadSrcLinkData.link);
			lpfnScrEditProc = (WNDPROC)SetWindowLong(GetDlgItem(dlgHandle, IDC_EDIT_SCRCOPY),
													 GWL_WNDPROC, (DWORD)editCtrlProc);
			return true;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_SCRUPLOAD_OK:
				case ID_SRCUPLOAD_CANCEL: {
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

LRESULT CALLBACK editCtrlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_LBUTTONDBLCLK: {
			//MessageBox(NULL, NULL, NULL, MB_OK);
			copyLinkToClipboard();
			return 0;
		}
	}
	return CallWindowProc(lpfnScrEditProc, hwnd, msg, wParam, lParam);
}
bool copyLinkToClipboard() {

	const size_t linkLen = _tcslen(uploadSrcLinkData.link) * sizeof(TCHAR) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, linkLen);
	memcpy(GlobalLock(hMem), uploadSrcLinkData.link, linkLen);
	GlobalUnlock(hMem);
	OpenClipboard(mainWindowHwnd);
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	return true;
}