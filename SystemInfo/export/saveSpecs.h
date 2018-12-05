#pragma once
#include "../core/SystemInfo.h"
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
	static bool saveAsText(HWND, SystemInfo*);
	static bool save(WORD command, RESULT_STRUCT res, HWND hwnd,
		SystemInfo *localMachine);
	saveSpecs();
	~saveSpecs();
};
void importAsXML(HWND);
