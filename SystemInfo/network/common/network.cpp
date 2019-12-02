#include <winsock2.h>
#include <iphlpapi.h>
#include <wininet.h>
#include <atlconv.h>
#include <atlbase.h> 
#include <atlstr.h>
#include <Ws2tcpip.h>
#include "network.h"
#include "../../util/utility.h"

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
		printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
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
	char buff[128] = { 0 };
	if (getIpAddress(buff)) {
		externalIpAddressDesc = L"Connected to the Internet";
		externalIp = fromChToWideStr(buff);
	} else {
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


#ifndef _DEBUG
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
			 0)) != NULL) {
		InternetReadFile(hFile, buffer, 128, &rSize);
		buffer[rSize] = '\0';
		strcpy(ipBuff, buffer);
		InternetCloseHandle(hFile);
		connectionRes = 1;
	} else {
		connectionRes = 0;
	}
	InternetCloseHandle(hInternet);
	//test fallback
#else
	strcpy(ipBuff, "1.1.1.1.");
#endif

	return connectionRes;
}

bool uploadImageToImgur(std::vector<BYTE> imagePayload) {
	std::string imagePayLoadStr(imagePayload.begin(), imagePayload.end());
	std::string Host = "https://www.api.imgur.com";
	int nret;
	WSADATA WsaDat;
	SOCKET theSocket;

	if (WSAStartup(MAKEWORD(2, 2), &WsaDat) != 0) {
		std::cout << "Failed To Initialize Winsock\n";
		return false;
	}

	ADDRINFO hints;
	ADDRINFO* pResult = nullptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_ALL;
	hints.ai_family = PF_INET;
	hints.ai_protocol = IPPROTO_IPV4;

	nret = getaddrinfo(Host.c_str(), nullptr, &hints, &pResult);
	if (nret != 0) {
		std::cout << "Failed To Do GetAddrInfo()\n";
		return false;
	}

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = *((ULONG*)&(((sockaddr_in*)pResult->ai_addr)->sin_addr));
	servAddr.sin_port = htons(8080);

	theSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (theSocket == INVALID_SOCKET) {
		std::cout << "Socket Is Invalid, Is Winsock Initialized?\n";
		return false;
	}

	nret = connect(theSocket, (struct sockaddr*)&servAddr, sizeof(servAddr));
	if (nret == SOCKET_ERROR) {
		std::cout << "Failed To Connect To Host\n";
		return false;
	}

	// Structure POST Data Properly
	std::string concatPostData;

	concatPostData += "image=" + imagePayLoadStr;

	// Construct HEADER
	std::string header;
	header.append("POST ");
	header.append("3/image/");
	header.append(" HTTP/1.1\r\n");
	header += "Host: " + Host + ":" + std::to_string(8080) + "\r\n";
	header += "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:40.0) Gecko/20100101 Firefox/40.0\r\n";
	header += "Referer: http://" + Host + "\r\n";
	header += "Content-Type: application/x-www-form-urlencoded\r\n";
	header += "Content-Length: " + std::to_string(concatPostData.length()) + "\r\n";
	header += "Accept-Charset: utf-8\r\n";
	header += "Connection: close\r\n\r\n";
	header += "Authorization: Client-ID ae55fac414fe09a";
	header += concatPostData + "\r\n";
	header += "\r\n";

	nret = send(theSocket, header.c_str(), header.length(), 0);
	if (nret == SOCKET_ERROR) {
		std::cout << "Failed To Send To Host\n";
		return false;
	}

	//std::string outBuf = loopRecieve(theSocket);

	// Display Data
	//std::cout << "Data:\n" << outBuf << "\n";

	closesocket(theSocket);
	WSACleanup();
	return true;
}

