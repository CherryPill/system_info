#pragma once
#include <iostream>
#include <vector>
#include <string.h>
#include <tchar.h>
#include "../const/appconst.h"
using namespace std;
class SoftwareInfo
{
private:
	TCHAR softwareName[STR_MAX];
	TCHAR softwareDescription[STR_MAX];
	TCHAR softwareVersion[STR_MAX];
	TCHAR softwareRepo[STR_MAX];
public:
	SoftwareInfo();
	~SoftwareInfo();
	TCHAR* getSoftwareName();
	TCHAR* getSoftwareDesc();
	TCHAR* getSoftwareVer();
	TCHAR* getSoftwareRepo();
	void setSoftwareDesc(TCHAR*);
	void setSoftwareVer(TCHAR*);
	void setSoftwareRepo(TCHAR*);
	void setSoftwareName(TCHAR*);
};

