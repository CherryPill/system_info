#include <Windows.h>
#include <tchar.h>
#include <vector>
#include "globalVars.h"
#include "utility.h"
#include "itemIDs.h"
#include "SystemInfo.h"
void centerWindow(POINT *upperLeftCorner) {
	(*upperLeftCorner).x = GetSystemMetrics(SM_CXSCREEN) / 2 - mainWindowWidth / 2;
	(*upperLeftCorner).y = GetSystemMetrics(SM_CYSCREEN) / 2 - mainWindowHeight / 2;
}
void trimNullTerminator(wstring &strToTrim) {
	strToTrim = strToTrim.erase(strToTrim.length());
}
std::string& BstrToStdString(const BSTR bstr, std::string& dst, int cp) {
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
wstring parseDiskStorageName(wstring modelName)
{
	wstring finalString = L"";
	if (modelName.find(L"WDC",0,3) != wstring::npos) {
		return storageMediumManufacturers[0];
	}
	else if (modelName.find(L"MHS",0,3) != wstring::npos) {
		return storageMediumManufacturers[2];
	}
	else if (modelName.find(L"HTS",0,3) != wstring::npos) {
		return storageMediumManufacturers[3];
	}
	else if (modelName.find(L"DTL") != wstring::npos) {
		return storageMediumManufacturers[5];
	}
	else if (modelName.find(L"ST",0,2) != wstring::npos || modelName.find(L"SC",0,2) != wstring::npos) {
		return storageMediumManufacturers[1];
	}
	else {
		return finalString;
	}
}
wstring convertUIntToString(UINT64 num) {
	wstring str;
	TCHAR *buff = new TCHAR[256];
	_stprintf(buff,L"%u",num);
	str = wstring(buff);
	delete buff;
	return str;
}
void trimWhiteSpace(wstring &str) {
	int whiteSpaceStart = str.find_last_not_of(L" \t");
	str.erase(whiteSpaceStart+1);
}
//Generates string in the following format: "sysinfo capture @ YYYY-MM-DD-HH:MM.required_extension"
void generateFileName(TCHAR *completeFileName, FILE_EXTENSION requiredExtension) {
	ZeroMemory(completeFileName, sizeof(completeFileName));
	TCHAR timeDateFileName[256];
	getCurrentDateTime(timeDateFileName);
	_tcscpy(completeFileName, _T("sysinfo capture "));
	_tcscat(completeFileName, timeDateFileName);
	_tcscat(completeFileName, savefileExtensions[(int)requiredExtension]);
}
void getCurrentDateTime(TCHAR *buffer) {
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);
	TCHAR minBuff[16];
	prependMinuteStr(currentTime.wMinute, minBuff);
	_stprintf(buffer, _T("%d-%d-%d @ %d %s"),
		currentTime.wYear,
		currentTime.wMonth,
		currentTime.wDay,
		currentTime.wHour,
		minBuff);
}
void prependMinuteStr(WORD min, TCHAR *minBuff) {
	if (min < 10) {
		_itow(min, minBuff, 10);
		wstring temp = wstring(minBuff);
		temp = L"0" + temp;
		_tcscpy(minBuff, temp.c_str());
	}
	else {
		_stprintf(minBuff, L"%d", min);
	}
}
//format: Friday, January 21, 2017 @ 0 00
void getCurrentDateTimeVerbose(TCHAR *buffer) {
	SYSTEMTIME currentTime;
	WORD min;
	TCHAR minBuff[16];
	GetLocalTime(&currentTime);
	prependMinuteStr(currentTime.wMinute, minBuff);
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
	MapWindowPoints(HWND_DESKTOP, windowHandle, (LPPOINT)&itemHandleDimensions, 2);
	UINT32 adjustedItemHeight;
	UINT32 adjustedYAxisOffset;
	adjustedItemHeight = innerItemsCount * 15;
	adjustedYAxisOffset = itemHandleDimensions.top + adjustedItemHeight;
	return adjustedYAxisOffset;
}
UINT32 isAdjustRequired(UINT32 ITEM_ID, SystemInfo *info)
{
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
		case AUDIO_INFO: {
			hardwareListSize = info->getAudio().size();
			break;
		}
		case NETWORK_INFO: {
			hardwareListSize = info->getNetworkAdapters().size();
			break;
		}
	}
	return hardwareListSize;
}
//this function forms a single string to display within the program window
//make HARDWARE_TYPE instead of harware_vector_type to process strings and vectors
wstring formListString(SystemInfo *currentMachine, HARDWARE_VECTOR_TYPE type, WRITE_OUT_TYPE wType) {
	wstring finalString;
	vector<wstring> values;
	wstring emptyValue;
	if (type == HARDWARE_VECTOR_TYPE::HARDWARE_VIDEO_ADAPTER) {
		values = currentMachine->getGPUDevices();
		emptyValue = itemStrings[5];
	}
	else if (type == HARDWARE_VECTOR_TYPE::HARDWARE_DISPLAY) {
		values = currentMachine->getDisplayDevices();
		emptyValue = itemStrings[6];
	}
	else if (type == HARDWARE_VECTOR_TYPE::HARDWARE_STORAGE) {
		values = currentMachine->getStorageMediums();
		emptyValue = itemStrings[7];
	}
	else if (type == HARDWARE_VECTOR_TYPE::HARDWARE_CDROM) {
		values = currentMachine->getCDROMDevices();
		emptyValue = itemStrings[8];
	}
	else if (type == HARDWARE_VECTOR_TYPE::HARDWARE_NETWORK) {
		vector<NetAdapter> detectedAdapters = currentMachine->getNetworkAdapters();
		for (auto iterator = detectedAdapters.begin();
			iterator != detectedAdapters.end();
			iterator++) {
			wstring completeString = iterator->getAdapterDesc()
			+ L": "+ iterator->getAdapterAdr();
			if (iterator->getAdapterType() != L"null") {
				completeString+= L" ("+ iterator->getAdapterType()+L")";
			}
			values.push_back(completeString);
		}
	}
	if (values.empty()) {
		return emptyValue + L" not detected";
	}
	else {
		for (auto iterator = values.begin();
		iterator != values.end();
			iterator++) {
			finalString.append((*iterator));
			if (wType == WRITE_OUT_TYPE::FILE_NON_TXT) {
				finalString.append(L"<br />");
			}
			else if (wType == WRITE_OUT_TYPE::FILE_TXT) {
				finalString.append(L"\n\t");
			}
			else {
				finalString.append(L"\n");
			}
		}
		return finalString;
	}
}
void openFileDiag(HWND mainWindow, FILE_EXTENSION extension, TCHAR *fullSavePath)  {
	OPENFILENAME fileName;
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&fileName, sizeof(fileName));
	fileName.lStructSize = sizeof(fileName);
	//fileName.lpstrFile = szFile;
	//fileName.lpstrFile[0] = '\0';
	fileName.hwndOwner = mainWindow;
	fileName.nMaxFile = sizeof(szFile);
	
	TCHAR *shortExtension = savefileExtensions[(int)extension];
	fileName.lpstrFilter = savefileExtensionsLong[(int)extension];
	fileName.Flags =
		OFN_PATHMUSTEXIST
		| OFN_OVERWRITEPROMPT
		| OFN_EXPLORER
		| OFN_HIDEREADONLY;
	TCHAR buffer[256];
	ZeroMemory(&buffer, sizeof(buffer));
	generateFileName(buffer,extension);
	fileName.lpstrFileTitle = buffer;
	fileName.lpstrFile = buffer;
	if (GetSaveFileName(&fileName)) {
		_tcscpy(fullSavePath, fileName.lpstrFile);
		//success
	}
	else{
		//failure
	}
}
void writeToFile(wofstream &fileStream, SystemInfo *info, int counter, WRITE_OUT_TYPE woType) {
	if (counter >= 5 && counter <= 9) {
		fileStream << formListString(info, static_cast<HARDWARE_VECTOR_TYPE>(counter % 5), woType).c_str();
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
		}
	}
}
wstring fromChToWideStr(char *value) {
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