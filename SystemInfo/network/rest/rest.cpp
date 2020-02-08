#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <sstream>
#include <wininet.h>
#include "atlstr.h"
#include "atlimage.h"
#include "rest.h"
#include "../../dialog/scrUploadDialog.h"
#include "../../export/screenCapture.h"

bool uploadImage(RESULT_STRUCT *res, TCHAR *fileName) {

	const TCHAR* UPLOAD_SERVER = _T("upload.gyazo.com");
	const TCHAR* UPLOAD_PATH = _T("/upload.cgi");
	const TCHAR* szTitle = L"Gyazo";

	const char*  sBoundary = "----BOUNDARYBOUNDARY----";		// boundary
	const char   sCrLf[] = { 0xd, 0xa, 0x0 };
	const TCHAR* szHeader =
		_T("Content-type: multipart/form-data; boundary=----BOUNDARYBOUNDARY----");

	std::ostringstream buf;
	std::string	idStr;	// ID


	buf << "--";
	buf << sBoundary;
	buf << sCrLf;
	buf << "content-disposition: form-data; name=\"id\"";
	buf << sCrLf;
	buf << sCrLf;
	buf << idStr;
	buf << sCrLf;

	// -- imagedata part
	buf << "--";
	buf << sBoundary;
	buf << sCrLf;
	buf << "content-disposition: form-data; name=\"imagedata\"; filename=\"gyazo.com\"";
	buf << sCrLf;
	//buf << "Content-type: image/png";
	//buf << sCrLf;
	buf << sCrLf;

	std::ifstream png;
	png.open(fileName, std::ios::binary);
	if (png.fail()) {
		MessageBox(NULL, _T("Could not open PNG file"), NULL, MB_ICONERROR | MB_OK);
		png.close();
		return FALSE;
	}
	buf << png.rdbuf();	//read all & append to buffer
	png.close();

	buf << sCrLf;
	buf << "--";
	buf << sBoundary;
	buf << "--";
	buf << sCrLf;

	std::string oMsg(buf.str());

	HINTERNET hSession = InternetOpen(szTitle,
									  INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (NULL == hSession) {
		MessageBox(NULL, _T("Cannot configure wininet"),
				   szTitle, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	HINTERNET hConnection = InternetConnect(hSession,
											UPLOAD_SERVER, INTERNET_DEFAULT_HTTP_PORT,
											NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
	if (NULL == hSession) {
		MessageBox(NULL, _T("Cannot initiate connection"),
				   szTitle, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	HINTERNET hRequest = HttpOpenRequest(hConnection,
										 _T("POST"), UPLOAD_PATH, NULL,
										 NULL, NULL, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, NULL);
	if (NULL == hSession) {
		MessageBox(NULL, _T("Cannot compose post request"),
				   szTitle, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	// Setting user-Agent
	const TCHAR* ua = _T("User-Agent: Gyazowin/1.0\r\n");
	BOOL bResult = HttpAddRequestHeaders(
		hRequest, ua, _tcslen(ua),
		HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	if (FALSE == bResult) {
		MessageBox(NULL, _T("Cannot set user agent"),
				   szTitle, MB_ICONERROR | MB_OK);
		return FALSE;
	}

	if (HttpSendRequest(hRequest,
						szHeader,
						lstrlen(szHeader),
						(LPVOID)oMsg.c_str(),
						(DWORD)oMsg.length())) {
		DWORD resLen = 8;
		TCHAR resCode[4];

		// status code
		HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, resCode, &resLen, 0);
		int ress = _tcscmp(resCode, L"200\0");
		if (ress) {
			// upload (status error)
			MessageBox(NULL, _T("Failed to upload (unexpected result code, under maintainance?)"),
					   szTitle, MB_ICONERROR | MB_OK);
		} else {
			DWORD idLen = 100;
			TCHAR newid[100];

			memset(newid, 0, idLen * sizeof(TCHAR));
			_tcscpy_s(newid, _T("X-Gyazo-Id"));

			HttpQueryInfo(hRequest, HTTP_QUERY_CUSTOM, newid, &idLen, 0);
			if (GetLastError() != ERROR_HTTP_HEADER_NOT_FOUND && idLen != 0) {

			}


			DWORD len;
			char  resbuf[1024];
			ZeroMemory(&resbuf, sizeof(resbuf));
			InternetReadFile(hRequest, (LPVOID)resbuf, 1024, &len);
			USES_CONVERSION;
			_tcscpy(res->__src, A2T(resbuf));

			return TRUE;
		}
	} else {
		return false;
	}
	return FALSE;
}

