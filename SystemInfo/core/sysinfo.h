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
		MEMTYPE,
		SPEED
	};
	
	enum class WMI_MB {
		MANUFACTURER,
		PRODUCT
	};
	enum class WMI_GPU {
		ADAPTER_RAM,
		NAME
	};
	enum class WMI_MONITOR {
		NAME
	};
	enum class WMI_DISPLAYCONFIG {
		NAME
	};
	enum class WMI_DISKDRIVE {
		CAPTION,
		SIZE
	};
	enum class WMI_OPTICALDISK {
		CAPTION
	};
	enum class WMI_SOUND {
		CAPTION
	};

	enum class WMI_CPU_PERF {
		PROC_TIME_PERCENTAGE
	};
	static unordered_map <wchar_t*, std::vector<LPCWSTR>> wmiClassStringsMap = {
		{ L"Win32_OperatingSystem", {L"Name", L"OSArchitecture"}},
		{ L"Win32_Processor", { L"Name", L"MaxClockSpeed", L"SocketDesignation"} },
		{ L"Win32_BaseBoard",{ L"Manufacturer", L"Product" } },
		{ L"Win32_PhysicalMemory", { L"Capacity", L"FormFactor", L"MemoryType", L"Speed" } },
		{ L"Win32_VideoController", { L"AdapterRAM", L"Name" } },
		{ L"Win32_DesktopMonitor", { L"Name"} },
		{ L"Win32_DisplayConfiguration", { L"DisplayFrequency"} },
		{ L"Win32_DiskDrive", { L"Caption", L"Size" } },
		{ L"Win32_CDROMDrive", { L"Caption" } },
		{ L"Win32_SoundDevice", { L"Caption"} },
		{ L"Win32_PerfFormattedData_PerfOS_Processor", { L"PercentProcessorTime"} }

	};
	int fillSystemInformation(SystemInfo *localMachine);
	int test();
	void fillCPUTemp(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	//hardware
	wstring getComputerType(void);
	void fillBIOS(SystemInfo *localMachine);
	void fillOS(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillCPU(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillMB(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillRAM(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillGPU(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillMonitor(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillStorage(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillCDROM(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillAudio(SystemInfo *localMachine, HRESULT hres, IWbemServices *pSvc, IWbemLocator *pLoc);
	void fillNetworkAdapters(SystemInfo *localMachine);
	void fillUptime(SystemInfo *localMachine);
	void fillDimensionsAndFrequency(HRESULT, IWbemServices*, IWbemLocator*, UINT*);
	wstring getSocket(HRESULT, IWbemServices*, IWbemLocator*);
	int getCpuUsagePercentage(void);
	IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator *pLoc, IWbemServices *pSvc, BSTR stringQuery);
	bstr_t buildQueryString(const wchar_t *wmiClass, vector<LPCWSTR> attrs);
	bstr_t buildQueryString(const wchar_t* wmiClass, vector<LPCWSTR> attrs, const wchar_t *whereClause);

	wstring getRamBySlot(HRESULT hres, IWbemServices*, IWbemLocator*, const vector<LPCWSTR>&);
#endif