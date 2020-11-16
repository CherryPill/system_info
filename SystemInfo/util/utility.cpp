#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <ShlObj.h>
#include <algorithm>
#include "../glb/globalVars.h"
#include "../util/utility.h"
#include "../const/itemIDs.h"
#include "../util/controlManager.h"

TCHAR* SavedUserSettingsHelper::configFileName = L"sys-info-config.conf";
TCHAR* SavedUserSettingsHelper::fullConfigFilePath;
void positionWindow(POINT* upperLeftCorner, INT32 windowWidth, INT32 windowHeight) {

	int offset = 0;
	if (applicationOpMode == APPLICATION_OPERATION_MODE::SNAPSHOT) {
		offset = 16;
	}
	(*upperLeftCorner).x = (GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2) + offset;
	(*upperLeftCorner).y = (GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2) + offset;
}

void centerWindow(POINT * upperLeftCorner) {
	(*upperLeftCorner).x = GetSystemMetrics(SM_CXSCREEN) / 2 - mainWindowWidth / 2;
	(*upperLeftCorner).y = GetSystemMetrics(SM_CYSCREEN) / 2 - mainWindowHeight / 2;
}

void trimNullTerminator(wstring & strToTrim) {
	strToTrim = strToTrim.erase(strToTrim.length());
}

std::wstring convertStringToWide(const std::string & as) {

	wchar_t* buf = new wchar_t[as.size() * 2 + 2];
	swprintf(buf, L"%S", as.c_str());
	std::wstring rval = buf;
	trimWhiteSpace(rval);
	delete[] buf;
	return rval;
}

std::string& BstrToStdString(const BSTR bstr, std::string & dst, int cp) {

	if (!bstr) {
		// define NULL functionality. I just clear the target.
		dst.clear();
		return dst;
	}
	// request content length in single-chars through a terminating
	//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
	//  so this will only convert through the first nullchar.
	int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
	if (res > 0) {
		dst.resize(res);
		WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
	}
	// no content. clear target
	else {
		dst.clear();
	}
	return dst;
}

wstring parseDiskStorageName(wstring modelName) {

	wstring finalString = L"";
	if (modelName.find(L"WDC", 0, 3) != wstring::npos) {
		return storageMediumManufacturers[0];
	}
	else if (modelName.find(L"MHS", 0, 3) != wstring::npos) {
		return storageMediumManufacturers[2];
	}
	else if (modelName.find(L"HTS", 0, 3) != wstring::npos) {
		return storageMediumManufacturers[3];
	}
	else if (modelName.find(L"DTL") != wstring::npos) {
		return storageMediumManufacturers[5];
	}
	else if (modelName.find(L"ST", 0, 2) != wstring::npos || modelName.find(L"SC", 0, 2) != wstring::npos) {
		return storageMediumManufacturers[1];
	}
	else {
		return finalString;
	}
}

vector<wstring> stringSplit(const wchar_t* s, wchar_t delimiter) {

	vector<wstring> res;
	do {
		const wchar_t* begin = s;
		while (*s != delimiter && *s) {
			s++;
		}
		res.push_back(wstring(begin, s));
		s += 5;
	} while (*s++ != 0);
	return res;
}

wstring convertUIntToString(UINT64 num) {
	wstring str;
	TCHAR* buff = new TCHAR[256];
	_stprintf(buff, L"%u", num);
	str = wstring(buff);
	delete buff;
	return str;
}

void trimWhiteSpace(wstring & str) {
	if (!str.empty()) {
		int whiteSpaceStart = str.find_last_not_of(L" \t");
		str.erase(whiteSpaceStart + 1);
		int whiteSpaceStartBeginning = str.find_first_not_of(L" \n\t");
		str.erase(str.begin(), str.end() - (str.length() - whiteSpaceStartBeginning));
	}
}



//Generates string in the following format: "sysinfo capture @ YYYY-MM-DD-HH:MM.required_extension"
void generateFileName(TCHAR * completeFileName, FILE_EXTENSION requiredExtension) {
	ZeroMemory(completeFileName, sizeof(completeFileName));
	TCHAR timeDateFileName[256];
	getCurrentDateTime(timeDateFileName);
	_tcscpy(completeFileName, _T("sysinfo-capture-"));
	_tcscat(completeFileName, timeDateFileName);
	_tcscat(completeFileName, savefileExtensions[(int)requiredExtension]);
}

void getCurrentDateTime(TCHAR * buffer) {
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);
	TCHAR finalTimeString[256];
	TCHAR strMinBuff[16];
	TCHAR strSecBuff[16];
	TCHAR strMonBuff[16];

	prependZeroToStr(currentTime.wMinute, strMinBuff);
	prependZeroToStr(currentTime.wSecond, strSecBuff);
	prependZeroToStr(currentTime.wMonth, strMonBuff);

	_stprintf(buffer, _T("%d-%s-%d_@_%d.%s.%s"),
		currentTime.wYear,
		strMonBuff,
		currentTime.wDay,
		currentTime.wHour,
		strMinBuff,
		strSecBuff);
}

void prependZeroToStr(WORD val, TCHAR * valBuff) {
	if (val < 10) {
		_itow(val, valBuff, 10);
		wstring temp = wstring(valBuff);
		temp = L"0" + temp;
		_tcscpy(valBuff, temp.c_str());
	}
	else {
		_stprintf(valBuff, L"%d", val);
	}
}

//format: Friday, January 21, 2017 @ 0 00
void getCurrentDateTimeVerbose(TCHAR * buffer) {
	SYSTEMTIME currentTime;
	WORD min;
	TCHAR minBuff[16];
	GetLocalTime(&currentTime);
	prependZeroToStr(currentTime.wMinute, minBuff);
	_stprintf(buffer, _T("%s, %s %d, %d @ %d:%s"),
		timeVerboseDaysOfWeek[currentTime.wDayOfWeek],
		timeVerboseMonths[currentTime.wMonth],
		currentTime.wDay,
		currentTime.wYear,
		currentTime.wHour,
		minBuff);
}

//adjusts item height based on the number of elements and returns updated offset
UINT32 adjustItemHeight(HWND windowHandle, UINT32 ITEM_ID, UINT32 innerItemsCount) {
	HWND itemHandle = GetDlgItem(windowHandle, ITEM_ID);
	RECT itemHandleDimensions;
	GetWindowRect(itemHandle, &itemHandleDimensions);
	MapWindowPoints(HWND_DESKTOP, windowHandle, (LPPOINT)& itemHandleDimensions, 2);
	UINT32 adjustedItemHeight;
	UINT32 adjustedYAxisOffset;
	adjustedItemHeight = innerItemsCount * 15;
	adjustedYAxisOffset = itemHandleDimensions.top + adjustedItemHeight + 5;
	return adjustedYAxisOffset;
}

UINT32 getInfoBoxItemCount(UINT32 ITEM_ID, SystemInfo * info) {
	UINT32 hardwareListSize = 0;
	switch (ITEM_ID) {
	case GPU_INFO: {
		hardwareListSize = info->getGPUDevices().size();
		break;
	}
	case MONITOR_INFO: {
		hardwareListSize = info->getDisplayDevices().size();
		break;
	}
	case STORAGE_INFO: {
		hardwareListSize = info->getStorageMediums().size();
		break;
	}
	case OPTICAL_INFO: {
		hardwareListSize = info->getCDROMDevices().size();
		break;
	}
	case NETWORK_INFO: {
		hardwareListSize = info->getNetworkAdaptersText().size();
		break;
	}
	default: {
		return 1; //single field like CPU or BIOS
		break;
	}
	}
	if (!hardwareListSize) {
		hardwareListSize = 1;
	}
	return hardwareListSize;
}

//this function forms a single string to display within the program window
//make HARDWARE_TYPE instead of harware_vector_type to process strings and vectors
wstring formListString(SystemInfo * currentMachine, OS_INFO_TYPES type, WRITE_OUT_TYPE wType) {
	wstring finalString = L"";
	vector<wstring> values;
	wstring emptyValue;
	if (type == OS_INFO_TYPES::HARDWARE_VIDEO_ADAPTER) {
		values = currentMachine->getGPUDevices();
		emptyValue = itemStrings[5];
	}
	else if (type == OS_INFO_TYPES::HARDWARE_DISPLAY) {
		values = currentMachine->getDisplayDevices();
		emptyValue = itemStrings[6];
	}
	else if (type == OS_INFO_TYPES::HARDWARE_STORAGE) {
		values = currentMachine->getStorageMediums();
		emptyValue = itemStrings[7];
	}
	else if (type == OS_INFO_TYPES::HARDWARE_CDROM) {
		values = currentMachine->getCDROMDevices();
		emptyValue = itemStrings[8];
	}
	else if (type == OS_INFO_TYPES::HARDWARE_NETWORK) {
		values = currentMachine->getNetworkAdaptersText();
		emptyValue = itemStrings[9];
	}
	if (values.empty()) {
		return emptyValue + L" not detected";
	}
	else {
		for (auto iterator = values.begin();
			iterator != values.end();
			iterator++) {
			finalString.append(writeOutPrefix[static_cast<int>(wType)]);
			finalString.append((*iterator));
			finalString.append(writeOutPostfix[static_cast<int>(wType)]);
		}
		return finalString;
	}
}

ACTION openFileDiag(HWND mainWindow,
	FILE_EXTENSION extension,
	TCHAR * fullOpenSavePath, int mode) {
	OPENFILENAME fileName;
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&fileName, sizeof(fileName));
	szFile[0] = L'\0';
	fileName.lStructSize = sizeof(fileName);
	fileName.hwndOwner = mainWindow;
	fileName.nMaxFile = sizeof(szFile);

	TCHAR* shortExtension = savefileExtensions[(int)extension];
	fileName.lpstrFilter = savefileExtensionsLong[(int)extension];
	if (mode) {
		fileName.Flags =
			OFN_PATHMUSTEXIST
			| OFN_OVERWRITEPROMPT
			| OFN_EXPLORER
			| OFN_HIDEREADONLY;
	}
	else {
		fileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	}
	if (mode) {
		TCHAR buffer[256];
		ZeroMemory(&buffer, sizeof(buffer));
		generateFileName(buffer, extension);
		fileName.lpstrFileTitle = buffer;
		fileName.lpstrFile = buffer;
		if (GetSaveFileName(&fileName)) {
			_tcscpy(fullOpenSavePath, fileName.lpstrFile);
			return ACTION::ACCEPTED;
		}
		else {
			return ACTION::CANCELED_OUT;
		}
	}
	else {

		fileName.lpstrFile = szFile;
		if (GetOpenFileName(&fileName)) {
			_tcscpy(fullOpenSavePath, fileName.lpstrFile);
			return ACTION::ACCEPTED;
			//success
		}
		else {
			return ACTION::CANCELED_OUT;
		}
	}

}

void writeToFile(wofstream & fileStream, SystemInfo * info, int counter, WRITE_OUT_TYPE woType) {
	if (counter >= 5 && counter <= 9) {
		fileStream << formListString(info, static_cast<OS_INFO_TYPES>(counter), woType).c_str();
	}
	else {
		switch (counter) {
		case 0: {
			fileStream << info->getBIOS().c_str();
			break;
		}
		case 1: {
			fileStream << info->getOS().c_str();
			break;
		}
		case 2: {
			fileStream << info->getCPU().c_str();
			break;
		}
		case 3: {
			fileStream << info->getMB().c_str();
			break;
		}
		case 4: {
			fileStream << info->getRAM().c_str();
			break;
		}
		case 10: {
			fileStream << info->getAudio().c_str();
			break;
		}
		case 11: {
			fileStream << info->getUptime().c_str();
			break;
		}
		case 12: {
			fileStream << info->getSnapshotGenDateTime().c_str();
			break;
		}
		}
	}
}

wstring fromChToWideStr(char* value) {
	char txtBuff[256] = { 0 };
	wstring wStr;
	wchar_t _wtxtBuff[256] = { 0 };
	strcpy(txtBuff, value);
	mbstowcs(_wtxtBuff, txtBuff, sizeof(txtBuff));
	wStr = wstring(_wtxtBuff);
	return wStr;
}

wstring fromIntToWideStr(int type) {
	wstring connType;
	switch (type) {
	case 1:
		connType = L"Other";
		break;
	case 6:
		connType = L"Ethernet";
		break;
	case 9:
		connType = L"Token Ring";
		break;
	case 15:
		connType = L"FDDI\n";
		break;
	case 23:
		connType = L"PPP\n";
		break;
	case 24:
		connType = L"Loopback";
		break;
	case 28:
		connType = L"Slip";
		break;
	case 71:
		connType = L"Wi-Fi";
		break;
	default:
		connType = L"Unknown type";
		break;
	}
	return connType;
}

wstring netAdapterStringWrapper(NetAdapter adapter) {

	wstring completeString = adapter.getAdapterDesc()
		+ L": " + adapter.getAdapterAdr();
	if (adapter.getAdapterType() != L"null") {
		completeString += L" (" + adapter.getAdapterType() + L")";
	}
	return completeString;
}

void getFileNameFromPath(TCHAR * fullPath, TCHAR * fileName) {
	TCHAR fullPathTempBuff[256] = { 0 };
	_tcscpy(fullPathTempBuff, fullPath);
#define fullPath fullPathTempBuff
	TCHAR* ptr = _tcstok(fullPath, _T("\\"));
	TCHAR* prevInstance;
	while (ptr != NULL) {
		prevInstance = ptr;
		ptr = _tcstok(NULL, _T("\\"));
	}
	_tcscpy(fileName, prevInstance);
#undef fullPath
}

ACTION fileIOCheck(wofstream & stream) {
	if (stream) {
		return ACTION::ACCEPTED;
	}
	else {
		return ACTION::__ERROR;
	}
}

void calculateTimeAndFormat(TCHAR * formattedTimeString) {

	UINT64 uptimeMilliseconds = GetTickCount64();
	UINT64 uptimeSeconds = 0;
	UINT64 uptimeMinutes = 0;
	UINT64 uptimeHours = 0;
	UINT64 uptimeDays = 0;

	uptimeSeconds = uptimeMilliseconds / 1000;
	uptimeMinutes = uptimeSeconds / 60;
	uptimeHours = uptimeSeconds / 3600;
	if (uptimeMinutes > 1) {
		uptimeSeconds -= uptimeMinutes * 60;
	}
	if (uptimeHours > 0) {
		uptimeMinutes -= uptimeHours * 60;
		if (uptimeHours > 24) {
			uptimeDays = uptimeHours / 24;
			uptimeHours -= uptimeDays * 24;
		}
	}
	if (uptimeDays != 0) {
		if (uptimeHours != 0) {
			if (uptimeHours > 1) {
				_stprintf(formattedTimeString,
					uptimeDays > 1 ? L"%llu days, %llu hrs, %llu mins, %llu seconds" :
					L"%llu day, %llu hrs, %llu mins, %llu seconds",
					uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);
			}
			else {
				_stprintf(formattedTimeString, uptimeDays > 1 ? L"%llu days, %llu hr"
					: L"%llu day, %llu hr, %llu mins, %ll seconds", uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);

			}
			_stprintf(formattedTimeString, L"%llu days, %llu hrs, %llu mins, %llu seconds",
				uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);
		}
		else {
			_stprintf(formattedTimeString, uptimeDays > 1 ? L"%llu days, %llu hrs, %llu mins, %llu seconds" :
				L"%llu day, %llu hrs, %llu mins, %llu seconds", uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);
		}
	}
	else {
		if (uptimeHours < 1) {
			_stprintf(formattedTimeString, L"%llu mins, %llu secs", uptimeMinutes, uptimeSeconds);
			//_stprintf(formattedTimeString, L"%s",L"Less than an hour");
		}
		else if (uptimeHours == 1) {
			_stprintf(formattedTimeString, L"%llu hr, %llu mins, %llu seconds", uptimeHours, uptimeMinutes, uptimeSeconds);
		}
		else {
			_stprintf(formattedTimeString, L"%llu hrs, %llu mins, %llu seconds", uptimeHours, uptimeMinutes, uptimeSeconds);
		}
	}
}


//opens external application to view exported data, opens 'open with' dialog
BOOL openDefAppForExpData(WORD command, RESULT_STRUCT * res) {
	std::wstring openUrl = res->src;
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.nShow = SW_SHOWNORMAL;
	sei.lpVerb = L"openas";
	sei.lpFile = openUrl.c_str();
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	return ShellExecuteEx(&sei);
}


void configAppData() {
	PWSTR sysAppDataPath;
	TCHAR fullPath[256];
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &sysAppDataPath))) {
		_tcscpy(fullPath, sysAppDataPath);
		_tcscat(fullPath, L"\\SystemInfo");
		CreateDirectoryW(fullPath, NULL);
		_tcscpy(sysInfoConfigDirectoryPath, fullPath);
	}
}

bool dirExists(LPCTSTR dirPath) {
	DWORD dwAttr = GetFileAttributes(dirPath);
	return (dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

bool createSimpleDirectory(TCHAR * path) {
	return CreateDirectoryW(path, NULL);
}

std::wstring convertWmiCapacityToGB(wstring val) {
	TCHAR tempChar[100];
	double cap;
	double capacity;
	double accumulatedRAM = 0;
	_tcscpy(tempChar, val.c_str());
	swscanf(tempChar, L"%lf", &cap);

	cap /= (pow(1024, 3));
	accumulatedRAM += cap;
	TCHAR capacityStrBuff[100];
	_stprintf(capacityStrBuff, _T("%.2lf"), accumulatedRAM);
	return wstring(capacityStrBuff);
}

void removeTabulation(std::wstring & str) {
	str.erase(std::remove(str.begin(), str.end(), L'\t'), str.end());
}

bool wcharEqualsPredicate(wchar_t a, wchar_t b) {
	return a == b && a == ' ';
}

void condenseSpaces(std::wstring & str) {
	std::wstring::iterator new_end =
		std::unique(str.begin(), str.end(), wcharEqualsPredicate);
	str.erase(new_end, str.end());
}

std::wstring getSystemErrorCodeMessageForErrorCode(DWORD errorCode) {
	TCHAR* errorCodeString = new TCHAR[256];
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& errorCode,
		0, NULL);
	return std::wstring(errorCodeString);
}

std::wstring formMessageForUIExportByExportAction(ControlManager::UI_MESS_RES_ICON res, DWORD act) {
	return (ControlManager::UI_messagesTxt[0] +
		ControlManager::UIMessageExportFileExtByUserExportModeCommandMap
		.at((ControlManager::UI_MESS_EXPORT_ACTION)static_cast<int>(act))
		[0] + L" file");
}

int GetEncoderClsid(const TCHAR * format, CLSID * pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
TCHAR* convertColorReftoHexColorString(COLORREF colorRef) {
	TCHAR* buff = new TCHAR[256]{ 0 };
	_stprintf(buff, L"#%X%X%X", GetRValue(colorRef),
		GetGValue(colorRef),
		GetBValue(colorRef));
	return buff;
}
UINT parseBool(BOOL val) {
	return val ? BST_CHECKED : BST_UNCHECKED;
}
BOOL parseCheckBoxState(UINT val) {
	return val == BST_CHECKED ? TRUE : FALSE;
}
SIZE *getAdjustedDimensionsForStaticCntrl(HWND hwnd, TCHAR * string) {
	SIZE *size = new SIZE();
	HDC hdc = GetDC(hwnd);
	HFONT font = (HFONT)SelectObject(hdc, helveticaFont);
	GetTextExtentPoint32(hdc, string, _tcslen(string), size);
	SelectObject(hdc, font);
	ReleaseDC(hwnd, hdc);

	return size;
}

void resizeWindow(HWND hwndParent, HWND cntrlHwnd, INT32 newWidth) {
	resizeWindow(hwndParent, cntrlHwnd, newWidth, -1);
}
void resizeWindow(HWND hwndParent, HWND cntrlHwnd, INT32 newWidth, INT32 newHeight) {
	RECT rect;
	GetWindowRect(cntrlHwnd, &rect);
	POINT ptl{ rect.left, rect.top };
	POINT pbr{ rect.right, rect.bottom, };
	ScreenToClient(hwndParent, &ptl);
	ScreenToClient(hwndParent, &pbr);
	
	if (newHeight == -1) {
		newHeight = rect.bottom - rect.top;
	}
	MoveWindow(cntrlHwnd, ptl.x, ptl.y, newWidth, newHeight, TRUE);
}