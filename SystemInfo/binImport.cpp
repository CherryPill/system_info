#include "utility.h"
#include "binImport.h"
void importData(SystemInfo* snapshotInstance) {
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(PROGRAM_DATA_IMPORT_LOCATION);
	if (res) {
		pugi::xml_node items = doc.child(L"hardwareinfo");
		pugi::xml_node itemInner;
		for (pugi::xml_node item = items.first_child(); item; item = item.next_sibling()) {
			wstring attrValue = item.first_attribute().value();
			wstring itemValue = item.first_child().value();
			trimWhiteSpace(itemValue);
			if (attrValue == L"BIOS/UEFI") {
				snapshotInstance->setBIOS(itemValue);
			}
			else if (attrValue == L"Operating System") {
				snapshotInstance->setOS(itemValue);
			}
			else if (attrValue == L"CPU") {
				snapshotInstance->setCPU(itemValue);
			}
			else if (attrValue == L"Motherboard") {
				snapshotInstance->setMB(itemValue);
			}
			else if (attrValue == L"RAM") {
				snapshotInstance->setRAM(itemValue);
			}
			else if (attrValue == L"Sound") {
				snapshotInstance->setAudio(itemValue);
			}
			else if (attrValue == L"Uptime") {
				snapshotInstance->setUptime(itemValue);
			}
			else if (attrValue == L"GPU") {
			for(pugi::xml_node subitem : item.children()){
					snapshotInstance->addGPUDevice(subitem.first_child().value());
				}
			}
			else if (attrValue == L"Display") {
				for (pugi::xml_node subitem : item.children()) {
					snapshotInstance->addDisplayDevice(subitem.first_child().value());
				}
			}
			else if (attrValue == L"Storage") {
				for (pugi::xml_node subitem : item.children()) {
					snapshotInstance->addStorageMedium(subitem.first_child().value());
				}
				
			}
			else if (attrValue == L"Optical drives") {
				for (pugi::xml_node subitem : item.children()) {
					snapshotInstance->addCDROMDevice(subitem.first_child().value());
				}
			}
			else if (attrValue == L"Network and Connections") {
				for (pugi::xml_node subitem : item.children()) {
					snapshotInstance->addNetworkAdapterText(subitem.first_child().value());
				}
			}
		}
	}
	else {
		MessageBox(NULL, (LPCWSTR)res.status, _T("Oops"), MB_OK | MB_ICONERROR);
	}
}
