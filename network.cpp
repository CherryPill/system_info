#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include "network.h"
#include "utility.h"

void NetAdapter::setAdapterDesc(wstring v) {
	(*this).adapterDesc = v;
}
void NetAdapter::setAdapterAdr(wstring v) {
	(*this).adapterAddr = v;
}
void NetAdapter::setAdapterGW(wstring v) {

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
			adapter.setAdapterDesc(desc);
			adapter.setAdapterAdr(ipAddr);
			localMachine->addNetworkAdapter(adapter);
			pAdapter = pAdapter->Next;
		}
	}
	int sentinel = 0xf;
}
