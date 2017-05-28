#pragma once
#include <string.h>
#include "SystemInfo.h"
using namespace std;
class SystemInfo;
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