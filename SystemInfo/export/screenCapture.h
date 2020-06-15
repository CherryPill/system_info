#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H
#include <Windows.h>
#include <gdiplus.h>
#include "../util/utility.h"
typedef struct VisibleAreaCoordsStruct {
	INT32 visibleAreaWidth;
	INT32 visibleAreaHeight;
};
ACTION takeScreenshot(HWND, SCR_SAVETYPE, RESULT_STRUCT*);
Gdiplus::Bitmap* getBitmapFromAreaCoords(HWND hwnd, VisibleAreaCoordsStruct&);
VisibleAreaCoordsStruct getVisibleClientArea(HWND hwnd);
Gdiplus::Bitmap* mergeBitmaps( std::vector<Gdiplus::Bitmap*>, INT32);

#endif // !SCREENCAPTURE_H
