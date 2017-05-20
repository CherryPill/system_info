#pragma once
#include <Windows.h>
#include <tchar.h>
typedef struct _RawSMBIOSData {
	BYTE	Used20CallingMethod;
	BYTE	SMBIOSMajorVersion;
	BYTE	SMBIOSMinorVersion;
	BYTE	DmiRevision;
	DWORD	Length;
	PBYTE	SMBIOSTableData;
} RawSMBIOSData, *PRawSMBIOSData;
typedef struct _SMBIOSHEADER_ {
	BYTE Type;
	BYTE Length;
	WORD Handle;
} SMBIOSHEADER, *PSMBIOSHEADER;
typedef struct _TYPE_0_ {
	SMBIOSHEADER	Header;
	UCHAR	Vendor;
	UCHAR	Version;
	UINT16	StartingAddrSeg;
	UCHAR	ReleaseDate;
	UCHAR	ROMSize;
	ULONG64 Characteristics;
	UCHAR	Extension[2]; // spec. 2.3
	UCHAR	MajorRelease;
	UCHAR	MinorRelease;
	UCHAR	ECFirmwareMajor;
	UCHAR	ECFirmwareMinor;
} BIOSInfo, *PBIOSInfo;
void DumpSMBIOSStruct(void *, UINT, TCHAR*);
const char* LocateStringA(const char* str, UINT i);
const char* toPointString(void* p);
const wchar_t* LocateStringW(const char* str, UINT i);