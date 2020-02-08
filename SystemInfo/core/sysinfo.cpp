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

//TODO: Conflate fillMB and fillCPU into one procedure since MB uses socket info available on Win32_Processor

void(*fillInfoFuncs[])(SystemInfo*, HRESULT, IWbemServices*, IWbemLocator*) =
{ fillOS, fillCPU, fillMB, fillRAM, fillGPU, fillMonitor,
fillStorage, fillCDROM, fillAudio };



//the actual function that does all the heavy work
int fillSystemInformation(SystemInfo *localMachine) {
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to initialize COM library"), _T("Fatal Error"), MB_OK);
		return 1;
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to initialize security"), _T("Fatal Error"), MB_OK);
		CoUninitialize();
		return 1;
	}

	//obtain the initial locator to WMI

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to create IWbemLocator object"), _T("Fatal Error"), MB_OK);
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Could not connect"), _T("Fatal Error"), MB_OK);
		pLoc->Release();
		CoUninitialize();
		return 1;
	}


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Could not set proxy blanket"), _T("Fatal Error"), MB_OK);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;
	}

	//use the IWbemServices pointer to make requests of WMI

	//temp
	fillCPUTemp(localMachine, hres, pSvc, pLoc);
	//hardware
	fillBIOS(localMachine);
	fillComputerType(localMachine);
	for (int x = 0; x < sizeof(fillInfoFuncs) / sizeof(fillInfoFuncs[0]); x++) {
		(*fillInfoFuncs[x])(localMachine, hres, pSvc, pLoc);
	}

	fillNetworkAdapters(localMachine);
	fillUptime(localMachine);

	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return 0;
}

void fillCPU(SystemInfo *localMachine,
			 HRESULT hres, IWbemServices *pSvc,
			 IWbemLocator *pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_Processor");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_Processor", queryAttrs));
	IWbemClassObject *pclsObj = NULL;
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
		if (processor.find(L"@", 0) == string::npos) {
			hr = pclsObj->Get(queryAttrs.at((int)WMI_CPU::MAXCLOCK), 0, &vtProp, 0, 0);
			maxClockInMhZ = vtProp.uintVal;
			maxClockInGhZ = (double)maxClockInMhZ / 1000;
			_stprintf(maxClockBuff, _T("%.1lf"), maxClockInGhZ);
			maxClock = wstring(maxClockBuff);
			fullCPUString = processor + L" @ " + maxClock + L" GHz";
		} else {
			fullCPUString = processor;
		}

		localMachine->setCPU(fullCPUString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillRAM(SystemInfo *localMachine,
			 HRESULT hres, IWbemServices *pSvc,
			 IWbemLocator *pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_PhysicalMemory");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_PhysicalMemory", queryAttrs));
	IWbemClassObject *pclsObj = NULL;
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
		wstring bankLabel;
		UINT32 bank;
		TCHAR bankLabelBuff[100];
		wstring bankStr;

		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::CAPACITY), 0, &vtProp, 0, 0);
		capacityStr = convertWmiCapacityToGB(vtProp.bstrVal);

		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::FORMFACTOR), 0, &vtProp, 0, 0);
		formFactor = vtProp.uintVal;
		formFactorStr = RAMFormFactors[formFactor];

		hr = pclsObj->Get(queryAttrs.at((int)WMI_RAM::SPEED), 0, &vtProp, 0, 0);
		clock = vtProp.uintVal;
		_stprintf(clockStrBuff, _T("%d"), clock);
		clockStr = wstring(clockStrBuff);
		localMachine->setRAM(capacityStr +
							 L" GB " + formFactorStr + L" " + clockStr + L"MHz");

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void fillOS(SystemInfo *localMachine,
			HRESULT hres, IWbemServices *pSvc,
			IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_OperatingSystem");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_OperatingSystem", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillMB(SystemInfo *localMachine,
			HRESULT hres, IWbemServices *pSvc,
			IWbemLocator *pLoc) {

	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_BaseBoard");
	IEnumWbemClassObject* pEnumerator = executeWQLQuery
	(hres, pLoc, pSvc, buildQueryString(L"Win32_BaseBoard", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillGPU(SystemInfo *localMachine,
			 HRESULT hres, IWbemServices *pSvc,
			 IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_VideoController");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_VideoController", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillMonitor(SystemInfo *localMachine,
				 HRESULT hres, IWbemServices *pSvc,
				 IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DesktopMonitor");
	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DesktopMonitor", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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
								IWbemServices *pSvc,
								IWbemLocator *pLoc, UINT *arr) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DisplayConfiguration");

	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DisplayConfiguration", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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


void fillStorage(SystemInfo *localMachine,
				HRESULT hres, IWbemServices *pSvc,
				IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_DiskDrive");

	IEnumWbemClassObject* pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_DiskDrive", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillCDROM(SystemInfo *localMachine,
			   HRESULT hres, IWbemServices *pSvc,
			   IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_CDROMDrive");

	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc,
								  buildQueryString(L"Win32_CDROMDrive", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillAudio(SystemInfo *localMachine,
			   HRESULT hres, IWbemServices *pSvc,
			   IWbemLocator *pLoc) {
	vector<LPCWSTR> queryAttrs = wmiClassStringsMap.at(L"Win32_SoundDevice");

	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator =
		executeWQLQuery(hres, pLoc, pSvc, buildQueryString(L"Win32_SoundDevice", queryAttrs));

	IWbemClassObject *pclsObj = NULL;
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

void fillUptime(SystemInfo *localMachine) {

	wstring uptimeStr;
	TCHAR formattedTimeString[256] = { 0 };

	calculateTimeAndFormat(formattedTimeString);

	uptimeStr = wstring(formattedTimeString);
	localMachine->setUptime(uptimeStr);
}

void fillBIOS(SystemInfo *localMachine) {
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
		} else {
			MessageBox(NULL, _T("Failed to fetch firmware tables"), _T("Fatal Error"), MB_OK);
		}
	} else {
		MessageBox(NULL, _T("Memory allocation failed"), _T("Fatal Error"), MB_OK);
	}
	localMachine->setBIOS(biosData);
}

void fillCPUTemp(SystemInfo *localMachine,
				 HRESULT hres, IWbemServices *pSvc,
				 IWbemLocator *pLoc) {
	//stub for now	
}

wstring getSocket(HRESULT hres, IWbemServices *pSvc,
				   IWbemLocator *pLoc) {
	wstring socket;
	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_Processor"));

	IWbemClassObject *pclsObj = NULL;
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

IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator *pLoc,
									  IWbemServices *pSvc, BSTR stringQuery) {
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		stringQuery,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	//test this behavior
	if (FAILED(hres)) {
		displayMessageGeneric(UI_MESS_RES::FAILURE,
							  L"Fatal error: Query to operating system failed");
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	} else {
		return pEnumerator;
	}
}

int test() {

	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to initialize COM library"), _T("Fatal Error"), MB_OK);
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to initialize security"), _T("Fatal Error"), MB_OK);
		CoUninitialize();
		return 1;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Failed to create IWbemLocator object"), _T("Fatal Error"), MB_OK);
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\WMI"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Could not connect"), _T("Fatal Error"), MB_OK);
		pLoc->Release();
		CoUninitialize();
		return 1;                // Program has failed.
	}

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres)) {
		MessageBox(NULL, _T("Could not set proxy blanket"), _T("Fatal Error"), MB_OK);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----


	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc,
								  bstr_t("SELECT * FROM MSAcpi_ThermalZoneTemperature"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
									   &pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		VariantInit(&vtProp);
		hr = pclsObj->Get(L"CurrentTemperature", 0, &vtProp, 0, 0);
		wstring socket = vtProp.bstrVal;
		displayMessageGeneric(UI_MESS_RES::SUCCESS, socket.c_str());
		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pEnumerator->Release();

	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return 0;
}

bstr_t buildQueryString(const wchar_t *wmiClass, vector<LPCWSTR> attrs) {
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
	int x = 0;
	return bstr_t(queryString);
}

void fillComputerType(SystemInfo *localMachine) {
	SYSTEM_POWER_CAPABILITIES systemPowerCapabilities;
	ZeroMemory(&systemPowerCapabilities, sizeof(systemPowerCapabilities));
	GetPwrCapabilities(&systemPowerCapabilities);
	systemPowerCapabilities.LidPresent
		?
		localMachine->setComputerType(PCType[0])
		:
		localMachine->setComputerType(PCType[1]);
}