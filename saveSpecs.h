#pragma once
#include "SystemInfo.h"
#include <tchar.h>
class saveSpecs
{
private:
	static const TCHAR *xmlDTD;
	static const TCHAR *htmlDTD;
	static const TCHAR *CSS;
	static const wchar_t *xmlHeaderComment;
public:
	static bool saveAsXML(HWND, SystemInfo*);
	static bool saveAsHTML(HWND, SystemInfo*);
	static bool saveAsBin(HWND, SystemInfo*);
	static bool saveAsText(HWND, SystemInfo*);
	saveSpecs();
	~saveSpecs();
};

