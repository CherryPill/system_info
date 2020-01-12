#pragma once
#include "../core/SystemInfo.h"
#include <tchar.h>
#include "../util/utility.h"
class saveSpecs {
	private:
	static const TCHAR *xmlDTD;
	static const TCHAR *htmlStartPoint;
	static const TCHAR *uniformComment;
	static const TCHAR *htmlCommentStart;
	static const TCHAR *htmlCommentEnd;
	static const TCHAR *CSSCommentStart;
	static const TCHAR *CSSCommentEnd;
	public:
	static ACTION saveAsXML(HWND, SystemInfo*, RESULT_STRUCT*);
	static ACTION saveAsHTML(HWND, SystemInfo*, RESULT_STRUCT*);
	static ACTION saveAsText(HWND, SystemInfo*, RESULT_STRUCT*);
	static ACTION save(WORD command,
					 RESULT_STRUCT *res,
					 HWND hwnd,
					 SystemInfo *localMachine);
	saveSpecs();
	~saveSpecs();
};
void importAsXML(HWND);
