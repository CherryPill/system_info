#include <iostream>
#include <string.h>
#include <fstream>
#include "globalVars.h"
#include "saveSpecs.h"
#include "SystemInfo.h"
#include "utility.h"
using namespace std;
const TCHAR *saveSpecs::xmlDTD = _T("<?xml version=\"1.0\" encoding=\"utf - 8\" standalone=\"yes\"?>\n");
const TCHAR *saveSpecs::htmlDTD = _T("<!DOCTYPE html>\n");
saveSpecs::saveSpecs()
{
	
}
bool saveSpecs::saveAsHTML(SystemInfo *info)
{
	
}
bool saveSpecs::saveAsXML(SystemInfo *info)
{
	ofstream xmlOutFile;
	xmlOutFile.open("D:\\as\\specs.xml", ofstream::out);
	xmlOutFile<<saveSpecs::xmlDTD;
	xmlOutFile<<_T("<hardwareinfo>\n");
	for(int x = 0;x<10;x++)
	{
		xmlOutFile<<_T("<item type=\"");
		if (x>=4 && x<=7)
		{
			formListString(info, static_cast<HARDWARE_VECTOR_TYPE>(x)).c_str();
		}
		else
		{
				
		}
		xmlOutFile<<itemStrings[x].c_str();
		xmlOutFile<<_T("\">contents</item>\n");
	}
	xmlOutFile<<_T("</hardwareinfo>\n");
	xmlOutFile.close();
	return true;
}
bool saveSpecs::saveAsBin(SystemInfo *info)
{
	return true;
}
bool saveSpecs::saveAsText(SystemInfo *info)
{
	return true;
}
saveSpecs::~saveSpecs()
{
}
