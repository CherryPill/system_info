#include <winsock2.h>
#include <iphlpapi.h>
#include <wininet.h>

#include <atlconv.h>
#include <atlbase.h> 
#include <atlstr.h>
#include "network.h"
#include "utility.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "wininet")
#pragma comment (lib, "ws2_32.lib")
char *getUniCastIP(PIP_ADAPTER_UNICAST_ADDRESS pUnicast) {
	SOCKET_ADDRESS actualAddress = pUnicast->Address;
	LPSOCKADDR sock = actualAddress.lpSockaddr;
	sockaddr_in *act = (sockaddr_in*)sock;
	char *uniCastAddress = inet_ntoa(act->sin_addr);
	return uniCastAddress;
}
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
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	DWORD dwRetValAddr = 0;
	UINT i;
	ULONG outBufLen = 0x3A98;

	pAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
	if (pAddresses == NULL) {
		printf ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
		exit(1);
	}
	dwRetValAddr = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen);
	USES_CONVERSION;
	if (dwRetValAddr == NO_ERROR) {
		pCurrAddresses = pAddresses;
		while (pCurrAddresses) {
			if (pCurrAddresses->IfType != 0x18 && pCurrAddresses->IfIndex != 0x83) {
				PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
				NetAdapter adapter = NetAdapter();
				wstring desc = wstring(pCurrAddresses->Description);
				wstring ipAddr = CA2W(getUniCastIP(pUnicast));
				wstring type = pCurrAddresses->FriendlyName;
				adapter.setAdapterDesc(desc);
				adapter.setAdapterAdr(ipAddr);
				adapter.setAdapterType(type);
				localMachine->addNetworkAdapterText(netAdapterStringWrapper(adapter));
			}
			pCurrAddresses = pCurrAddresses->Next;
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
	//localMachine->addNetworkAdapter(extIpPlaceHolder);
	localMachine->addNetworkAdapterText(netAdapterStringWrapper(extIpPlaceHolder));
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
	
	strcpy(ipBuff, "1.1.1.1.");
	return connectionRes;
}

