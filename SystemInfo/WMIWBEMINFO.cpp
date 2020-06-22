#include "WMIWBEMINFO.h"


WMIWBEMINFO* WMIWBEMINFO::currentInstance;

WMIWBEMINFO* WMIWBEMINFO::getWMIWBEMINFOInstance() {
	if (currentInstance == nullptr) {
		currentInstance = new WMIWBEMINFO();
	}
	return currentInstance;
}

IWbemLocator* WMIWBEMINFO::getWbemLocator() {
	return this->pLoc;
}

IWbemServices* WMIWBEMINFO::getWbemServices() {
	return this->pSvc;
}

HRESULT WMIWBEMINFO::getHres() {
	return this->hres;
}

WMIWBEMINFO::~WMIWBEMINFO() {
	this->pSvc->Release();
	this->pLoc->Release();
	CoUninitialize();
}
