#include "atlstr.h"
#include "SMBIOS.h"
void DumpSMBIOSStruct(void *Addr, UINT Len, TCHAR *outData) {
	LPBYTE p = (LPBYTE)(Addr);
	const DWORD lastAddress = ((DWORD)p) + Len;
	PSMBIOSHEADER pHeader;
	pHeader = (PSMBIOSHEADER)p;
	PBIOSInfo pBIOS = (PBIOSInfo)p;
	const char *str = toPointString(p);
	TCHAR *biosVendor = (TCHAR*)LocateStringW(str, pBIOS->Vendor);
	_tcscpy(outData, biosVendor);
	TCHAR *biosVersion = (TCHAR*)LocateStringW(str, pBIOS->Version);
	_tcscat(outData, _T(" v."));
	_tcscat(outData, biosVersion);
}
const char* toPointString(void* p) {
	return (char*)p + ((PSMBIOSHEADER)p)->Length;
}
const wchar_t* LocateStringW(const char* str, UINT i) {
	static wchar_t buff[2048];
	const char *pStr = LocateStringA(str, i);
	SecureZeroMemory(buff, sizeof(buff));
	MultiByteToWideChar(CP_UTF8, 0, pStr, strlen(pStr), buff, sizeof(buff));
	return buff;
}
const char* LocateStringA(const char* str, UINT i) {
	static const char strNull[] = "Null String";
	if (0 == i || 0 == *str) return strNull;
	while (--i) {
		str += strlen((char*)str) + 1;
	}
	return str;
}