#include <winsock2.h>
#include <iphlpapi.h>
#include <wininet.h>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "wininet")
#include "network.h"
#include "utility.h"

void NetAdapter::setAdapterDesc(wstring v) {
	(*this).adapterDesc = v;
}
void NetAdapter::setAdapterAdr(wstring v) {
	(*this).adapterAddr = v;
}
void NetAdapter::setAdapterGW(wstring v) {
	//stub
}
wstring NetAdapter::getAdapterDesc(void) {
	return (*this).adapterDesc;
}
wstring NetAdapter::getAdapterAdr(void) {
	return (*this).adapterAddr;
}
wstring NetAdapter::getAdapterGW(void) {
	return L"null";
}
void NetAdapter::setAdapterType(wstring v) {
	(*this).adapterType = v;
}
wstring NetAdapter::getAdapterType(void) {
	return (*this).adapterType;
}
void getNetworkAdapters(SystemInfo* localMachine) {
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			NetAdapter adapter = NetAdapter();
			wstring desc = fromChToWideStr(pAdapter->Description);
			wstring ipAddr = fromChToWideStr(pAdapter->IpAddressList.IpAddress.String);
			wstring type = fromIntToWideStr(pAdapter->Type);
			adapter.setAdapterDesc(desc);
			adapter.setAdapterAdr(ipAddr);
			adapter.setAdapterType(type);
			localMachine->addNetworkAdapter(adapter);
			pAdapter = pAdapter->Next;
		}
	}
	NetAdapter extIpPlaceHolder = NetAdapter();
	wstring externalIpAddressDesc;
	wstring externalIp = L"Unable to fetch IP";
	wstring type = L"null";
	char buff[128] = {0};
	if (getIpAddress(buff)) {
		externalIpAddressDesc = L"Connected to the Internet";
		externalIp = fromChToWideStr(buff);
	}
	else {
		externalIpAddressDesc = L"Not connected to the Internet";
	}
	extIpPlaceHolder.setAdapterDesc(externalIpAddressDesc);
	extIpPlaceHolder.setAdapterAdr(externalIp);
	extIpPlaceHolder.setAdapterType(type);
	localMachine->addNetworkAdapter(extIpPlaceHolder);
	int sentinel = 0xf;
}
int getIpAddress(char *ipBuff) {
	int connectionRes = 1;
	HINTERNET hInternet, hFile;
	DWORD rSize;
	char *buffer = new char[128];
	
	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if ((hFile = 
		InternetOpenUrl(
		hInternet, 
		L"https://api.ipify.org", 
		NULL, 
		0, 
		INTERNET_FLAG_RELOAD,
		0)) != NULL)
	{
		hFile = InternetOpenUrl(hInternet, L"https://api.ipify.org", NULL, 0, INTERNET_FLAG_RELOAD, 0);
		InternetReadFile(hFile, buffer, 128, &rSize);
		buffer[rSize] = '\0';
		strcpy(ipBuff, buffer);
		InternetCloseHandle(hFile);
		connectionRes = 1;
	}
	else {
		connectionRes = 0;
	}
	InternetCloseHandle(hInternet);
	//test fallback
	//strcpy(ipBuff, "1.1.1.1.");
	return connectionRes;
}