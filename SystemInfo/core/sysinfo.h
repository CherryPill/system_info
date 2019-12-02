#ifndef SYSINFOSTRUCT_H
	#define SYSINFOSTRUCT_H
	#include <comdef.h>
	#include <Wbemidl.h>
	#include "../const/appconst.h"
	#include "../core/SystemInfo.h"
	using namespace std;
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
	wstring getActualPhysicalMemory(HRESULT, IWbemServices*, IWbemLocator*);
	wstring getSocket(HRESULT, IWbemServices*, IWbemLocator*);
	void getDimensionsAndFrequency(HRESULT, IWbemServices*, IWbemLocator*, UINT*);
	void getAudio(SystemInfo *localMachine, HRESULT, IWbemServices*, IWbemLocator*);
	IEnumWbemClassObject* executeWQLQuery(HRESULT hres, IWbemLocator *pLoc, IWbemServices *pSvc, BSTR stringQuery);
#endif