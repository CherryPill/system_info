#include "WMIWBEMINFO.h"



WMIWBEMINFO* WMIWBEMINFO::getWMIWBEMInfoInstance() {
	if (!currentInstance) {
		currentInstance = new WMIWBEMINFO();
	}
	return currentInstance;
}

WMIWBEMINFO::~WMIWBEMINFO()
{
}
