#include "SoftwareInfo.h"



SoftwareInfo::SoftwareInfo()
{
}


SoftwareInfo::~SoftwareInfo()
{
}


string SoftwareInfo::getSoftwareName() {
	return this->softwareName;
}


string SoftwareInfo::getSoftwareDesc() {
	return this->softwareDescription;
}


string SoftwareInfo::getSoftwareVer() {
	return this->softwareVersion;
}


string SoftwareInfo::getSoftwareRepo() {
	return this->softwareRepo;
}


void SoftwareInfo::setSoftwareDesc(string desc) {
	this->softwareDescription = desc;
}


void SoftwareInfo::setSoftwareVer(string ver) {
	this->softwareVersion = ver;
}


void SoftwareInfo::setSoftwareRepo(string repo) {
	this->softwareRepo = repo;
}


void SoftwareInfo::setSoftwareName(string name) {
	this->softwareName = name;
}
