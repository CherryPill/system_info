#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include "../glb/globalVars.h"
#include "../resource.h"
typedef struct UPLOAD_SRC_LINK_DATA {
	TCHAR link[256];
};
extern UPLOAD_SRC_LINK_DATA uploadSrcLinkData;
BOOL CALLBACK scrDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK editCtrlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool copyLinkToClipboard();
