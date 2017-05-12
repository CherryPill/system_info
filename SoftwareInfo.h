#pragma once
#include <iostream>
#include <vector>
#include <string.h>
using namespace std;
class SoftwareInfo
{
private:
	string softwareName;
	string softwareDescription;
	string softwareVersion;
	string softwareRepo;
public:
	SoftwareInfo();
	~SoftwareInfo();
	string getSoftwareName();
	string getSoftwareDesc();
	string getSoftwareVer();
	string getSoftwareRepo();
	void setSoftwareDesc(string);
	void setSoftwareVer(string);
	void setSoftwareRepo(string);
	void setSoftwareName(string);
};

