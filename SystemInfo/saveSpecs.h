#pragma once
#include "SystemInfo.h"
#include <tchar.h>
class saveSpecs
{
private:
	static const TCHAR *xmlDTD;
	static const TCHAR *htmlStartPoint;
	static const TCHAR *uniformComment;
	static const TCHAR *htmlCommentStart;
	static const TCHAR *htmlCommentEnd;
	static const TCHAR *CSSCommentStart;
	static const TCHAR *CSSCommentEnd;
public:
	static bool saveAsXML(HWND, SystemInfo*);
	static bool saveAsHTML(HWND, SystemInfo*);
	static bool saveAsBin(HWND, SystemInfo*);
	static bool saveAsText(HWND, SystemInfo*);
	saveSpecs();
	~saveSpecs();
};

