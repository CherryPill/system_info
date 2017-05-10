#include <Windows.h>
#include <tchar.h>
#include <vector>
#include "globalVars.h"
#include "utility.h"
#include "itemIDs.h"
#include "SystemInfo.h"
void centerWindow(POINT *upperLeftCorner)
{
	(*upperLeftCorner).x = GetSystemMetrics(SM_CXSCREEN) / 2 - mainWindowWidth / 2;
	(*upperLeftCorner).y = GetSystemMetrics(SM_CYSCREEN) / 2 - mainWindowHeight / 2;
}
void trimNullTerminator(string &strToTrim)
{
	strToTrim = strToTrim.erase(strToTrim.length()-1);
}
std::wstring& BStrToWStdString(const BSTR bstr, std::wstring& dst, int cp)
{
	if (!bstr)
	{
		// define NULL functionality. I just clear the target.
		dst.clear();
		return dst;
	}
	// request content length in single-chars through a terminating
	//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
	//  so this will only convert through the first nullchar.
	int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
	if (res > 0)
	{
		dst.resize(res);
		WideCharToMultiByte(cp, 0, bstr, -1, (LPSTR)&dst[0], res, NULL, NULL);
	}
	else
	{    // no content. clear target
		dst.clear();
	}
	return dst;
}
std::string& BstrToStdString(const BSTR bstr, std::string& dst, int cp)
{
	if (!bstr)
	{
		// define NULL functionality. I just clear the target.
		dst.clear();
		return dst;
	}
	// request content length in single-chars through a terminating
	//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
	//  so this will only convert through the first nullchar.
	int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
	if (res > 0)
	{
		dst.resize(res);
		WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
	}
	else
	{    // no content. clear target
		dst.clear();
	}
	return dst;
}
string parseDiskStorageName(string modelName)
{
	string finalString = "";
	if (modelName.find("WDC",0,3) != string::npos)
	{
		return storageMediumManufacturers[0];
	}
	else if (modelName.find("MHS",0,3) != string::npos)
	{
		return storageMediumManufacturers[2];
	}
	else if (modelName.find("HTS",0,3) != string::npos)
	{
		return storageMediumManufacturers[3];
	}
	else if (modelName.find("DTL") != string::npos)
	{
		return storageMediumManufacturers[5];
	}
	else if (modelName.find("ST",0,2) != string::npos || modelName.find("SC",0,2) != string::npos)
	{
		return storageMediumManufacturers[1];
	}
	else
	{
		return finalString;
	}
}
string convertUIntToString(UINT64 num)
{
	string str;
	char *buff = new char[256];
	sprintf(buff,"%u",num);
	str = string(buff);
	delete buff;
	return str;
}
void trimWhiteSpace(string &str)
{
	int whiteSpaceStart = str.find_last_not_of(" \t");
	str.erase(whiteSpaceStart+1);
}
//Generates string in the following format: "sysinfo capture @ YYYY-MM-DD-HH:MM.required_extension"
void generateFileName(TCHAR *completeFileName, FILE_EXTENSION requiredExtension)
{
	ZeroMemory(completeFileName, sizeof(completeFileName));
	TCHAR timeDateFileName[256];
	getCurrentDateTime(timeDateFileName);
	_tcscpy(completeFileName, _T("sysinfo capture "));
	_tcscat(completeFileName, timeDateFileName);
	_tcscat(completeFileName, savefileExtensions[(int)requiredExtension]);
}
void getCurrentDateTime(TCHAR *buffer)
{
	SYSTEMTIME currentTime;
	GetSystemTime(&currentTime);
	_stprintf(buffer, _T("%d-%d-%d %d %d"),
		currentTime.wYear,
		currentTime.wMonth,
		currentTime.wDay,
		currentTime.wHour,
		currentTime.wMinute);
}
//adjusts item height based on the number of elements and returns updated offset
UINT32 adjustItemHeight(HWND windowHandle, UINT32 ITEM_ID, UINT32 innerItemsCount)
{
	HWND itemHandle = GetDlgItem(windowHandle, ITEM_ID);
	RECT itemHandleDimensions;
	GetWindowRect(itemHandle, &itemHandleDimensions);
	MapWindowPoints(HWND_DESKTOP, windowHandle, (LPPOINT)&itemHandleDimensions, 2);
	UINT32 adjustedItemHeight;
	UINT32 adjustedYAxisOffset;
	adjustedItemHeight = innerItemsCount * 15;
	
	//SetWindowPos(itemHandle,NULL,itemHandleDimensions.left, 
		//itemHandleDimensions.top,
			//			itemHandleDimensions.right,
				//		adjustedItemHeight,NULL);
	adjustedYAxisOffset = itemHandleDimensions.top + adjustedItemHeight + 10;
	return adjustedYAxisOffset;
}
UINT32 isAdjustRequired(UINT32 ITEM_ID, SystemInfo *info)
{
	UINT32 hardwareListSize = 0;
	switch (ITEM_ID)
	{
		case GPU_INFO:
		{
			hardwareListSize = info->getGPUDevices().size();
			break;
		}
		case MONITOR_INFO:
		{
			hardwareListSize = info->getDisplayDevices().size();
			break;
		}
		case STORAGE_INFO:
		{
			hardwareListSize = info->getStorageMediums().size();
			break;
		}
		case OPTICAL_INFO:
		{
			hardwareListSize = info->getCDROMDevices().size();
			break;
		}
		case AUDIO_INFO:
		{
			hardwareListSize = info->getAudio().size();
			break;
		}
	}
	return hardwareListSize;
}
//this function forms a single string to display within the program window
//make HARDWARE_TYPE instead of harware_vector_type to process strings and vectors
string formListString(SystemInfo *currentMachine, HARDWARE_VECTOR_TYPE type)
{
	string finalString;
	vector<string> values;
	string emptyValue;
	switch (type)
	{
	case HARDWARE_VECTOR_TYPE::HARDWARE_DISPLAY:
	{
		values = currentMachine->getDisplayDevices();
		emptyValue = itemStrings[5];
		break;
	}
	case HARDWARE_VECTOR_TYPE::HARDWARE_STORAGE:
	{
		values = currentMachine->getStorageMediums();
		emptyValue = itemStrings[6];
		break;
	}
	case HARDWARE_VECTOR_TYPE::HARDWARE_VIDEO_ADAPTER:
	{
		values = currentMachine->getGPUDevices();
		emptyValue = itemStrings[4];
		break;
	}
	case HARDWARE_VECTOR_TYPE::HARDWARE_CDROM:
	{
		values = currentMachine->getCDROMDevices();
		emptyValue = itemStrings[7];
		break;
	}
	}
	if (values.empty())
	{
		return emptyValue + " not detected";
	}
	else
	{
		for (auto iterator = values.begin();
		iterator != values.end();
			iterator++)
		{
			finalString.append((*iterator));
			finalString.append("\n");
		}
		return finalString;
	}
}
void openFileDiag(HWND mainWindow, FILE_EXTENSION extension, TCHAR *fullSavePath) 
{
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
	if (GetSaveFileName(&fileName))
	{
		_tcscpy(fullSavePath, fileName.lpstrFile);
		//success
	}
	else
	{
		//failure
	}
			



}