#include "SoftwareInfo.h"
SoftwareInfo *shippedSoftWare;
SoftwareInfo::SoftwareInfo() {
	this->setSoftwareName(_T("SystemInfo"));
	this->setSoftwareDesc(_T("Hardware information tool for windows"));
	this->setSoftwareRepo(_T("https://github.com/CherryPill/system_info"));
	this->setSoftwareVer(_T("1.0 (stable) [64-bit]"));
}
SoftwareInfo::~SoftwareInfo() {
}
TCHAR* SoftwareInfo::getSoftwareName() {
	return this->softwareName;
}
TCHAR* SoftwareInfo::getSoftwareDesc() {
	return this->softwareDescription;
}
TCHAR* SoftwareInfo::getSoftwareVer() {
	return this->softwareVersion;
}
TCHAR* SoftwareInfo::getSoftwareRepo() {
	return this->softwareRepo;
}
void SoftwareInfo::setSoftwareDesc(TCHAR* desc) {
	_tcscpy(this->softwareDescription, desc);
}
void SoftwareInfo::setSoftwareVer(TCHAR* ver) {
	_tcscpy(this->softwareVersion, ver);
}
void SoftwareInfo::setSoftwareRepo(TCHAR* repo) {
	_tcscpy(this->softwareRepo, repo);
}
void SoftwareInfo::setSoftwareName(TCHAR* name) {
	_tcscpy(this->softwareName, name);
}
