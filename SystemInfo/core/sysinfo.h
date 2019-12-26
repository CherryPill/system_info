#ifndef SYSINFOSTRUCT_H
	#define SYSINFOSTRUCT_H
	#include <comdef.h>
	#include <Wbemidl.h>
	#include <unordered_map>
	#include "../const/appconst.h"
	#include "../core/SystemInfo.h"
	using namespace std;

	enum class WMI_OS {
		NAME,
		ARCHITECTURE
	};

	enum class WMI_CPU {
		NAME,
		MAXCLOCK,
		SOCKET_DESIGNATION
	};

	enum class WMI_RAM  {
		CAPACITY,
		FORMFACTOR,
		BANKLABEL,
		MEMTYPE,
		SPEED
	};
	
	enum class WMI_MB {
		MANUFACTURER,
		PRODUCT
	};
	static unordered_map <wchar_t*, std::vector<LPCWSTR>> wmiClassStringsMap = {
		{ L"Win32_OperatingSystem", {L"Name", L"OSArchitecture"}},
		{ L"Win32_Processor", { L"Name", L"MaxClockSpeed", L"SocketDesignation"} },
		{ L"Win32_BaseBoard",{ L"Manufacturer", L"Product" } },
		{ L"Win32_PhysicalMemory", { L"Capacity", L"FormFactor", L"BankLabel", L"MemoryType", L"Speed" } },
		{ L"Win32_VideoController", { L"AdapterRAM", L"Name" } },
		{ L"Win32_DesktopMonitor", { L"Name"} },
		{ L"Win32_DisplayConfiguration", { L"DisplayFrequency"} },
		{ L"Win32_DiskDrive", { L"Caption", L"Size" } },
		{ L"Win32_CDROMDrive", { L"Caption" } },
		{ L"Win32_SoundDevice", { L"Caption"} },
	};
	int getSystemInformation(SystemInfo *localMachine);
	int test();
	void getBIOS(SystemInfo *localMachine);
	void getCPUInfo(void);
	void getCPUTemp(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getCPU(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getRAM(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getOS(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getMB(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getGPU(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getMonitor(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getStorage(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getCDROM(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	void getUptime(SystemInfo *localMachine);
	wstring getSocket(HRESULT, IWbemServices*, IWbemLocator*);
	void getDimensionsAndFrequency(HRESULT, IWbemServices*, IWbemLocator*, UINT*);
	void getAudio(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator *pLoc, IWbemServices *pSvc, BSTR stringQuery);
	bstr_t buildQueryString(const wchar_t *wmiClass, vector<LPCWSTR> attrs);
#endif