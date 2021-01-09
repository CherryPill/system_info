#include <Windows.h>
#include <tchar.h>
#include <intrin.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <sstream>
#include <atlconv.h>
#include <powerbase.h>
#include "../network/common/network.h"
#include "../glb/globalVars.h"
#include "../core/sysinfo.h"
#include "../core/SystemInfo.h"
#include "../util/utility.h"
#include "../SMBIOS.h"
#include "../mainWindowProcedure.h"
#include "../core/WMIWBEMINFO.h"
#include "../util/controlManager.h"

//TODO: Conflate fillMB and fillCPU into one procedure since MB uses socket info available on Win32_Processor

void(*fillInfoFuncs[])(SystemInfo*, HRESULT, IWbemServices*, IWbemLocator*) =
{ fillOS, fillCPU, fillMB, fillRAM, fillGPU, fillMonitor,
fillStorage, fillCDROM, fillAudio };


WMIWBEMINFO* wmiWbemInfo = wmiWbemInfo->getWMIWBEMINFOInstance();
//the actual function that does all the heavy work
int fillSystemInformation(SystemInfo* localMachine) {
	fillBIOS(localMachine);
	for (int x = 0; x < sizeof(fillInfoFuncs) / sizeof(fillInfoFuncs[0]); x++) {
		(*fillInfoFuncs[x])(localMachine, wmiWbemInfo->getHres(), 
			wmiWbemInfo->getWbemServices(), wmiWbemInfo->getWbemLocator());
	}

	fillNetworkAdapters(localMachine);
	fillUptime(localMachine);

	return 0;
}

void filterInformationOffSettings(SystemInfo* localMachine) {
	if (glbUserSettings->getIpAddrBehavior() == IP_ADDR_BEHAVIOR::DISABLED) {
		localMachine->getNetworkAdaptersTextRef().pop_back();
	}
}

void fillCPU(SystemInfo* localMachine,
	HRESULT hres, IWbemServices* pSvc,
	IWbemLocator* pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_Processor");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_Processor", queryAttrs));
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(queryAttrs.at((int)WMI_CPU::NAME), 0, &vtProp, 0, 0);
		wstring fullCPUString; //name + @clock
		wstring maxClock;
		double maxClockInGhZ;
		uint32_t maxClockInMhZ;
		TCHAR maxClockBuff[10];
		wstring processor;
		processor = vtProp.bstrVal;
		trimNullTerminator(processor);
		trimWhiteSpace(processor);
		removeTabulation(processor);
		condenseSpaces(processor);
		if (processor.find(L"@", 0) == string::npos) {
			hr = pclsObj->Get(queryAttrs.at((int)WMI_CPU::MAXCLOCK), 0, &vtProp, 0, 0);
			maxClockInMhZ = vtProp.uintVal;
			maxClockInGhZ = (double)maxClockInMhZ / 1000;
			_stprintf(maxClockBuff, _T("%.1lf"), maxClockInGhZ);
			maxClock = wstring(maxClockBuff);
			fullCPUString = processor + L" @ " + maxClock + L" GHz";
		}
		else {
			fullCPUString = processor;
		}

		localMachine->setCPU(fullCPUString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

wstring getActualAvailableRAM(HRESULT hres,
	IWbemServices* pSvc,
	IWbemLocator* pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_ComputerSystem");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_ComputerSystem", queryAttrs));
	IWbemClassObject* pclsObj = NULL;

	ULONG uReturn = 0;
	double actualCapacity = 0;
	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(L"TotalPhysicalMemory", 0, &vtProp, 0, 0);



		wstring temp;
		TCHAR tempChar[100];
		temp = vtProp.bstrVal;
		_tcscpy(tempChar, temp.c_str());
		swscanf(tempChar, L"%lf", &actualCapacity);

		actualCapacity /= (pow(1024, 3));
		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pEnumerator->Release();
	TCHAR capacityStrBuff[100];
	_stprintf(capacityStrBuff, _T("%.2lf"), actualCapacity);
	return wstring(capacityStrBuff);

}

void fillRAM(SystemInfo* localMachine,
	HRESULT hres, IWbemServices* pSvc,
	IWbemLocator* pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_PhysicalMemory");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_PhysicalMemory", queryAttrs));
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		// format: gb channel ddr @ mhz (no timings yet)

		wstring clockStr;
		UINT32 clock;
		UINT16 formFactor;
		wstring name;
		wstring capacityStr;
		wstring formFactorStr;
		wstring memoryTypeStr;
		UINT16 memoryType;
		uint64_t capacity;
		double capacityDouble;
		TCHAR capacityStrBuff[10];
		TCHAR clockStrBuff[10];
		wstring actualCapacityStr;

		capacityStr = getRamBySlot(hres, pSvc, pLoc, queryAttrs);
		actualCapacityStr = getActualAvailableRAM(hres, pSvc, pLoc);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::FORMFACTOR), 0, &vtProp, 0, 0);
		formFactor = vtProp.uintVal;
		formFactorStr = RAMFormFactors[formFactor];

		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::MEMTYPE), 0, &vtProp, 0, 0);
		memoryType = vtProp.uintVal;
		memoryTypeStr = RAMMemoryTypes[memoryType];

		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::SPEED), 0, &vtProp, 0, 0);
		clock = vtProp.uintVal;
		_stprintf(clockStrBuff, _T("%d"), clock);
		clockStr = wstring(clockStrBuff);
		
		localMachine->setRAM(capacityStr +
			L" GB " + (glbUserSettings->getShowUsableRam() ? L"(" + actualCapacityStr + L" GB usable) " : L"") + formFactorStr + L" " +
			(memoryTypeStr.length() == 0 ? L"" : memoryTypeStr + L" ") +
			clockStr + L"MHz");

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillOS(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_OperatingSystem");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_OperatingSystem", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(queryAttrs.at((int)WMI_OS::NAME), 0, &vtProp, 0, 0);
		wstring fullOSstring;
		string OSName;
		wstring OSArchitecture;
		wstring OSNameWide;

		OSNameWide = vtProp.bstrVal;
		hr = pclsObj->Get(queryAttrs.at((int)WMI_OS::ARCHITECTURE), 0, &vtProp, 0, 0);
		OSArchitecture = vtProp.bstrVal;
		int garbageIndex = OSNameWide.find(L"|");
		OSNameWide = OSNameWide.erase(garbageIndex, OSNameWide.length() - garbageIndex);
		fullOSstring = OSNameWide + L" " + OSArchitecture;

		localMachine->setOS(fullOSstring);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillMB(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_BaseBoard");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_BaseBoard", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(queryAttrs.at((int)WMI_MB::MANUFACTURER), 0, &vtProp, 0, 0);

		wstring manufacturer;
		wstring product;
		wstring socket;
		manufacturer = vtProp.bstrVal;
		manufacturer.erase(manufacturer.length());
		trimWhiteSpace(manufacturer);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_MB::PRODUCT), 0, &vtProp, 0, 0);

		product = vtProp.bstrVal;
		localMachine->setMB(manufacturer + L" " + product + L" (" + getSocket(hres, pSvc, pLoc) + L")");

		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillGPU(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_VideoController");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_VideoController", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(queryAttrs.at((int)WMI_GPU::ADAPTER_RAM), 0, &vtProp, 0, 0);
		wstring finalAdapterString;
		wstring name;
		UINT32 vramBytes;
		wstring vrammegabytesStr;
		TCHAR vRamCharBuff[50];
		double vRAMmegaBytes;

		vramBytes = vtProp.uintVal;
		vRAMmegaBytes = (double)vramBytes / pow(1024, 2);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_GPU::NAME), 0, &vtProp, 0, 0);
		name = vtProp.bstrVal;
		name.erase(name.length());
		trimNullTerminator(name);
		_stprintf(vRamCharBuff, _T("%.0lf MB"), vRAMmegaBytes);
		vrammegabytesStr = wstring(vRamCharBuff);
		finalAdapterString = name + L" " + vrammegabytesStr;

		localMachine->addGPUDevice(finalAdapterString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillMonitor(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DesktopMonitor");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DesktopMonitor", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		//Output format:
		//<Monitor name> (widthxheight@<hertzCount>Hz)
		wstring finalMonitorString;
		wstring monitorName;
		wstring resAndFreqStr;
		TCHAR resAndFreqBuff[50];
		UINT32 dimensionsAndFrequency[3];
		fillDimensionsAndFrequency(hres, pSvc, pLoc, dimensionsAndFrequency);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_MONITOR::NAME), 0, &vtProp, 0, 0);
		monitorName = vtProp.bstrVal;
		trimNullTerminator(monitorName);
		_stprintf(resAndFreqBuff, L"(%dx%d@%dHz)", dimensionsAndFrequency[0],
			dimensionsAndFrequency[1],
			dimensionsAndFrequency[2]);
		resAndFreqStr = wstring(resAndFreqBuff);
		finalMonitorString = monitorName + resAndFreqStr;
		localMachine->addDisplayDevice(finalMonitorString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}
void fillDimensionsAndFrequency(HRESULT hres,
	IWbemServices * pSvc,
	IWbemLocator * pLoc, UINT * arr) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DisplayConfiguration");

	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DisplayConfiguration", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	double accumulatedRAM = 0;
	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		arr[0] = GetSystemMetrics(SM_CXSCREEN);
		arr[1] = GetSystemMetrics(SM_CYSCREEN);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_DISPLAYCONFIG::NAME), 0, &vtProp, 0, 0);
		arr[2] = vtProp.uintVal;


		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}


void fillStorage(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DiskDrive");

	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DiskDrive", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		wstring storageFullString;
		wstring manufacturerName;
		wstring modelName;
		wstring capacityStr;
		UINT64 capacityBytes;
		wstring capacityGiBStr;
		UINT64 capacityGiBDbl;
		TCHAR capacity[256];
		hr = pclsObj->Get(queryAttrs.at((int)WMI_DISKDRIVE::CAPTION), 0, &vtProp, 0, 0);

		//product name
		modelName = vtProp.bstrVal;
		manufacturerName = parseDiskStorageName(modelName);
		trimNullTerminator(modelName);
		trimWhiteSpace(manufacturerName);
		hr = pclsObj->Get(queryAttrs.at((int)WMI_DISKDRIVE::SIZE), 0, &vtProp, 0, 0);

		capacityStr = vtProp.bstrVal;
		capacityBytes = stoull(capacityStr);
		capacityGiBDbl = capacityBytes / pow(1024, 3);
		capacityGiBStr = convertUIntToString(capacityGiBDbl);

		storageFullString = capacityGiBStr + L" GB " + manufacturerName + (manufacturerName == L"" ? L"" : L" ") + modelName;

		localMachine->addStorageMedium(storageFullString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillCDROM(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_CDROMDrive");

	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc,
		buildQueryString(L"Win32_CDROMDrive", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		wstring cdRomCaption;
		hr = pclsObj->Get(queryAttrs.at((int)WMI_OPTICALDISK::CAPTION), 0, &vtProp, 0, 0);

		cdRomCaption = vtProp.bstrVal;
		trimNullTerminator(cdRomCaption);
		condenseSpaces(cdRomCaption);
		localMachine->addCDROMDevice(cdRomCaption);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillAudio(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_SoundDevice");

	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_SoundDevice", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		wstring soundCaption;
		hr = pclsObj->Get(queryAttrs.at((int)WMI_SOUND::CAPTION), 0, &vtProp, 0, 0);

		soundCaption = vtProp.bstrVal;
		trimNullTerminator(soundCaption);
		localMachine->setAudio(soundCaption);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillUptime(SystemInfo * localMachine) {

	wstring uptimeStr;
	TCHAR formattedTimeString[256] = { 0 };

	calculateTimeAndFormat(formattedTimeString);

	uptimeStr = wstring(formattedTimeString);
	localMachine->setUptime(uptimeStr);
}

void fillBIOS(SystemInfo * localMachine) {
	DWORD needBufferSize = 0;
	TCHAR biosData[256];
	SecureZeroMemory(&biosData, sizeof(biosData));
	const BYTE byteSignature[] = { 'B', 'M', 'S', 'R' };
	const DWORD Signature = *((DWORD*)byteSignature);
	LPBYTE pBuff = NULL;
	needBufferSize = GetSystemFirmwareTable(Signature, 0, NULL, 0);
	pBuff = (LPBYTE)malloc(needBufferSize);
	if (pBuff) {
		if (GetSystemFirmwareTable(Signature, 0, pBuff, needBufferSize)) {
			const PRawSMBIOSData pDMIData = (PRawSMBIOSData)pBuff;
			DumpSMBIOSStruct(&(pDMIData->SMBIOSTableData), pDMIData->Length, biosData);
		}
		else {
			GenericMessageOK()
				.withMessage(L"Fatal Error. Failed to fetch firmware tables")
				->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
				->display();
		}
	}
	else {
		GenericMessageOK()
			.withMessage(L"Fatal Error. Memory allocation failed")
			->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
			->display();
	}
	_tcscat(biosData, (wchar_t*)getComputerType().c_str());
	localMachine->setBIOS(biosData);
}

void fillCPUTemp(SystemInfo * localMachine,
	HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	//stub for now	
}

wstring getSocket(HRESULT hres, IWbemServices * pSvc,
	IWbemLocator * pLoc) {
	wstring socket;
	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_Processor"));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(L"SocketDesignation", 0, &vtProp, 0, 0);
		socket = vtProp.bstrVal;
		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pEnumerator->Release();
	return socket;
}

IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator * pLoc,
	IWbemServices * pSvc, BSTR stringQuery) {
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		stringQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	//test this behavior
	if (FAILED(hres)) {
		GenericMessageOK()
			.withMessage(L"Fatal error: Query to operating system failed")
			->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
			->display();

		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}
	else {
		return pEnumerator;
	}
}

bstr_t buildQueryString(const wchar_t* wmiClass, vector<LPCWSTR> attrs) {

	WCHAR queryString[256] = { 0 };
	wcscpy(queryString, L"SELECT ");
	auto it = attrs.begin();
	for (auto it = attrs.begin(); it != attrs.end(); it++) {
		wcscat(queryString, *it);
		if (!((attrs.end() - it == 1))) {
			wcscat(queryString, L", ");
		}
	}
	wcscat(queryString, L" FROM ");
	wcscat(queryString, wmiClass);
	return bstr_t(queryString);
}

bstr_t buildQueryString(const wchar_t* wmiClass,
	vector<LPCWSTR> attrs,
	const wchar_t* whereClause) {
	bstr_t mainQuery = buildQueryString(wmiClass, attrs);
	bstr_t mainQueryWithWhereClause = mainQuery;
	if (whereClause != NULL) {
		mainQueryWithWhereClause += SysAllocString(L" WHERE ");
		mainQueryWithWhereClause += SysAllocString(whereClause);
	}
	return mainQueryWithWhereClause;
}

wstring getComputerType() {
	SYSTEM_POWER_CAPABILITIES systemPowerCapabilities;
	ZeroMemory(&systemPowerCapabilities, sizeof(systemPowerCapabilities));
	GetPwrCapabilities(&systemPowerCapabilities);
	return systemPowerCapabilities.LidPresent
		?
		PCType[0]
		:
		PCType[1];
}

wstring getRamBySlot(HRESULT hres,
	IWbemServices *pSvc,
	IWbemLocator *pLoc,
	const vector<LPCWSTR> &queryAttrs) {

	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_PhysicalMemory", queryAttrs));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	double accumulatedRAM = 0;
	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}
		VARIANT vtProp;
		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::CAPACITY), 0, &vtProp, 0, 0);

		double cap;
		double capacity;

		wstring temp;
		TCHAR tempChar[100];
		temp = vtProp.bstrVal;
		_tcscpy(tempChar, temp.c_str());
		swscanf(tempChar, L"%lf", &cap);

		cap /= (pow(1024, 3));
		accumulatedRAM += cap;
		VariantClear(&vtProp);
	}
	TCHAR capacityStrBuff[100];
	_stprintf(capacityStrBuff, _T("%.2lf"), accumulatedRAM);
	return wstring(capacityStrBuff);
}

unsigned int getCpuUsagePercentage() {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_PerfFormattedData_PerfOS_Processor");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(wmiWbemInfo->getHres(), 
		wmiWbemInfo->getWbemLocator(), 
		wmiWbemInfo->getWbemServices(), 
		buildQueryString(L"Win32_PerfFormattedData_PerfOS_Processor", queryAttrs, L"Name='_Total'"));

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	//if -1, then WMI for some reason didn't get the data from the tables
	int cpuUsage = -1;
		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}

			VARIANT vtProp;

			hr = pclsObj->Get(queryAttrs.at((int)WMI_CPU_PERF::PROC_TIME_PERCENTAGE), 0, &vtProp, 0, 0);

			if (WBEM_S_NO_ERROR != hr) {
				if (pclsObj) {
					VariantClear(&vtProp);
					pclsObj->Release(); pclsObj = NULL;
				}
				throw std::exception("Failed to get info from perf table");
				break;
			}
			cpuUsage = std::stoi(vtProp.bstrVal);

			VariantClear(&vtProp);

			pclsObj->Release(); pclsObj = NULL;
		}
	return cpuUsage;
}

