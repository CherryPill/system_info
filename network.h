#pragma once
#include <string.h>
#include "SystemInfo.h"
using namespace std;
class SystemInfo;
//used as a place holder for the external internet address (ipv4)
class NetAdapter {
private:
	wstring adapterDesc;
	wstring adapterAddr;
	wstring adapterGW;
public:
	NetAdapter() {}
	void setAdapterDesc(wstring);
	void setAdapterAdr(wstring);
	void setAdapterGW(wstring);
	wstring getAdapterDesc(void);
	wstring getAdapterAdr(void);
	wstring getAdapterGW(void);
};
void getNetworkAdapters(SystemInfo*);
int getIpAddress(char *ipBuff);
