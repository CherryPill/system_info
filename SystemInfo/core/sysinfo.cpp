#include <Windows.h>
#include <tchar.h>
#include <intrin.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <sstream>
#include <atlconv.h>
#include "../network/common/network.h"
#include "../glb/globalVars.h"
#include "../core/sysinfo.h"
#include "../core/SystemInfo.h"
#include "../util/utility.h"
#include "../SMBIOS.h"
#include "../mainWindowProcedure.h"

#pragma comment(lib, "wbemuuid.lib")

void getCPUTemp(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
//hardware
void getBIOS(SystemInfo *localMachine);
void getOS(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getCPU(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getMB(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getRAM(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getGPU(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getMonitor(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getStorage(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getCDROM(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getAudio(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
void getNetworkAdapters(SystemInfo *localMachine);
void getUptime(SystemInfo *localMachine);

void (*getInfoFuncs[])(SystemInfo*,HRESULT, IWbemServices*, IWbemLocator*) = 
							{getOS, getCPU, getMB, getRAM, getGPU, getMonitor, 
							getStorage, getCDROM, getAudio};





//the actual function that does all the work
int getSystemInformation(SystemInfo *localMachine)
{
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


	if (FAILED(hres)){
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

	if (FAILED(hres)){
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
		return 1;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


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

	//temp
	getCPUTemp(localMachine, hres, pSvc, pLoc);
	//hardware
	getBIOS(localMachine);

	for (int x = 0;x< sizeof(getInfoFuncs)/sizeof(getInfoFuncs[0]);x++) {
		(*getInfoFuncs[x])(localMachine, hres, pSvc, pLoc);
	}
	
	getNetworkAdapters(localMachine);
	getUptime(localMachine);
	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return 0;   // Program successfully completed.
}

void getCPU(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc) {
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_Processor"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		wstring fullCPUString; //name + @clock
		wstring maxClock;
		double maxClockInGhZ;
		uint32_t maxClockInMhZ;
		TCHAR maxClockBuff[10];
		wstring processor;
		processor = vtProp.bstrVal;
		trimNullTerminator(processor);
		trimWhiteSpace(processor);
		if (processor.find(L"@",0) == string::npos) {
			hr = pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
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

void getRAM(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc) {
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_PhysicalMemory"));
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		//format
		//gb channel ddr @ mhz (no timings yet)

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
		capacityStr = getActualPhysicalMemory(hres, pSvc,pLoc);
		
		hr = pclsObj->Get(L"FormFactor", 0, &vtProp, 0, 0);
		formFactor = vtProp.uintVal;
		formFactorStr = RAMFormFactors[formFactor];
		
		hr = pclsObj->Get(L"BankLabel", 0, &vtProp, 0, 0);
		bank = vtProp.uintVal;
		_stprintf(bankLabelBuff, _T("%d"), bank);
		bankStr = wstring(bankLabelBuff);

		hr = pclsObj->Get(L"MemoryType", 0, &vtProp, 0, 0);
		memoryType = vtProp.uintVal;
		memoryTypeStr = RAMMemoryTypes[memoryType];
		hr = pclsObj->Get(L"Speed", 0, &vtProp, 0, 0);	
		clock = vtProp.uintVal;
		_stprintf(clockStrBuff, _T("%d"), clock);
		clockStr = wstring(clockStrBuff);
		localMachine->setRAM(capacityStr +
		L" GB " + formFactorStr + L" "+memoryTypeStr+L" "+clockStr+L"MHz");

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void getOS(SystemInfo *localMachine, 
			HRESULT hres, IWbemServices *pSvc,
			IWbemLocator *pLoc) {
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_OperatingSystem"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		wstring fullOSstring;
		string OSName;
		wstring OSArchitecture;
		wstring OSNameWide;

		OSNameWide = vtProp.bstrVal;
		hr = pclsObj->Get(L"OSArchitecture", 0, &vtProp, 0, 0);
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

void getMB(SystemInfo *localMachine,
		HRESULT hres, IWbemServices *pSvc,
		IWbemLocator *pLoc) {
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_BaseBoard"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);

		wstring manufacturer;
		wstring product;
		wstring socket;
		manufacturer = vtProp.bstrVal;
		manufacturer.erase(manufacturer.length());
		trimWhiteSpace(manufacturer);
		hr = pclsObj->Get(L"Product", 0, &vtProp, 0, 0);
		
		product = vtProp.bstrVal;
		localMachine->setMB(manufacturer + L" " + product + L" (" + getSocket(hres, pSvc, pLoc)+L")");

		VariantClear(&vtProp);
		pclsObj->Release();
	}
	pEnumerator->Release();
}

wstring getActualPhysicalMemory(HRESULT hres, 
	IWbemServices *pSvc,
	IWbemLocator *pLoc){
	wstring ram;
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_PhysicalMemory"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;
	double accumulatedRAM = 0;
	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(L"Capacity", 0, &vtProp, 0, 0);
		double cap;
		double capacity;
		
		wstring temp;
		TCHAR tempChar[100];
		temp = vtProp.bstrVal;
		_tcscpy(tempChar, temp.c_str());
		swscanf(tempChar, L"%lf", &cap);
		
		cap/= (pow(1024, 3));
		accumulatedRAM+=cap;
		VariantClear(&vtProp);

		pclsObj->Release();
	}
	TCHAR capacityStrBuff[100];
	_stprintf(capacityStrBuff, _T("%.2lf"), accumulatedRAM);
	ram = wstring(capacityStrBuff);
	pEnumerator->Release();
	return ram;
}

void getGPU(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc){
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_VideoController"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		hr = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
		wstring finalAdapterString;
		wstring name;
		UINT32 vramBytes;
		wstring vrammegabytesStr;
		TCHAR vRamCharBuff[50];
		double vRAMmegaBytes;
		
		vramBytes = vtProp.uintVal;
		vRAMmegaBytes = (double) vramBytes/pow(1024,2);
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		name = vtProp.bstrVal;
		name.erase(name.length());
		trimNullTerminator(name);
		_stprintf(vRamCharBuff,_T("%.0lf MB"),vRAMmegaBytes);
		vrammegabytesStr = wstring(vRamCharBuff);
		finalAdapterString = name+L" "+vrammegabytesStr;

		localMachine->addGPUDevice(finalAdapterString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void getMonitor(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc){
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_DesktopMonitor"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		//DELL P2014H(DP) (1600x900@60Hz)
		wstring finalMonitorString;
		wstring monitorName;
		wstring resAndFreqStr;
		TCHAR resAndFreqBuff[50];
		UINT32 dimensionsAndFrequency[3];
		getDimensionsAndFrequency(hres, pSvc, pLoc,dimensionsAndFrequency);
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
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
void getDimensionsAndFrequency(HRESULT hres,
	IWbemServices *pSvc,
	IWbemLocator *pLoc, UINT *arr)
{
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_DisplayConfiguration"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;
	double accumulatedRAM = 0;
	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		arr[0] = GetSystemMetrics(SM_CXSCREEN);
		arr[1] = GetSystemMetrics(SM_CYSCREEN);
		hr = pclsObj->Get(L"DisplayFrequency", 0, &vtProp, 0, 0);
		arr[2] = vtProp.uintVal;

	
		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}
void getStorage(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc) {
	IEnumWbemClassObject* pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_DiskDrive"));

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
		// Get the value of the Name property
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);

		//product - m5a97 r2.0
		modelName = vtProp.bstrVal;
		manufacturerName = parseDiskStorageName(modelName);
		trimNullTerminator(modelName);
		trimWhiteSpace(manufacturerName);
		hr = pclsObj->Get(L"Size", 0, &vtProp, 0, 0);

		capacityStr = vtProp.bstrVal;
		capacityBytes = stoull(capacityStr);
		capacityGiBDbl = capacityBytes/pow(1024,3);
		capacityGiBStr = convertUIntToString(capacityGiBDbl);
		
		storageFullString = capacityGiBStr + L" GB " + manufacturerName + (manufacturerName==L""?L"":L" ")+ modelName;

		localMachine->addStorageMedium(storageFullString);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}
void getCDROM(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc)
{
	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_CDROMDrive"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		wstring cdRomCaption;
		// Get the value of the Name property
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);

		cdRomCaption = vtProp.bstrVal;
		trimNullTerminator(cdRomCaption);
		localMachine->addCDROMDevice(cdRomCaption);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}
void getAudio(SystemInfo *localMachine,
	HRESULT hres, IWbemServices *pSvc,
	IWbemLocator *pLoc)
{
	IEnumWbemClassObject* pEnumerator = NULL;
	pEnumerator = executeWQLQuery(hres, pLoc, pSvc, bstr_t("SELECT * FROM Win32_SoundDevice"));

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		wstring soundCaption;
		// Get the value of the Name property
		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);

		soundCaption = vtProp.bstrVal;
		trimNullTerminator(soundCaption);
		localMachine->setAudio(soundCaption);

		VariantClear(&vtProp);

		pclsObj->Release();
	}
	pEnumerator->Release();
}

void getUptime(SystemInfo *localMachine) {
		
		wstring uptimeStr;
		TCHAR formattedTimeString[256] = {0};

		calculateTimeAndFormat(formattedTimeString);
		
		uptimeStr = wstring(formattedTimeString);
		localMachine->setUptime(uptimeStr);
}

void getBIOS(SystemInfo *localMachine) {
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
			MessageBox(NULL, _T("Failed to fetch firmware tables"), _T("Fatal Error"), MB_OK);
		}
	}
	else {
		MessageBox(NULL, _T("Memory allocation failed"), _T("Fatal Error"), MB_OK);
	}
	localMachine->setBIOS(biosData);
}
void getCPUTemp(SystemInfo *localMachine,
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

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
			&pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		
		// Get the value of the Name property
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
	if (FAILED(hres)){
		displayMessageGeneric(UI_MESS_RES::FAILURE, 
		L"Query for operating system failed");
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}
	else {
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

    //cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


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

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        VariantInit(&vtProp);
        // Get the value of the Name property
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

    return 0;   // Program successfully completed.
}