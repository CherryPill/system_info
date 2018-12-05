#pragma once
#include <string.h>

#include "../../core/SystemInfo.h"
using namespace std;
class SystemInfo;
//also used as a place holder for the external internet address (ipv4)
class NetAdapter {
private:
	wstring adapterDesc;
	wstring adapterAddr;
	wstring adapterGW;
	wstring adapterType;
public:
	NetAdapter() {}
	void setAdapterType(wstring);
	void setAdapterDesc(wstring);
	void setAdapterAdr(wstring);
	void setAdapterGW(wstring);
	wstring getAdapterType(void);
	wstring getAdapterDesc(void);
	wstring getAdapterAdr(void);
	wstring getAdapterGW(void);
};
void getNetworkAdapters(SystemInfo*);
int getIpAddress(char *ipBuff);
bool uploadImageToImgur(std::vector<BYTE> imagePayload);
