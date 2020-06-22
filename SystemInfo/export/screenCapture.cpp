#include <Windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "atlimage.h"
#include "../export/screenCapture.h"
#include "../util/utility.h"
#include "../dialog/scrUploadDialog.h"
#include "../network/rest/rest.h"


ACTION takeScreenshot(HWND hwnd, SCR_SAVETYPE scrSaveType, RESULT_STRUCT *res) {
	
	//init GDI+
	ULONG_PTR gdiPlusToken;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartup(&gdiPlusToken, &input, nullptr);

	CLSID pngEncoderCLSID;

	if (GetEncoderClsid(
		encoderImgTypesToStringMap.at(ENCODER_IMG_TYPES::IMG_PNG).c_str(),
		&pngEncoderCLSID) == -1) {
		pngEncoderCLSID = glbPngFallbackHardCodedEncoderClsID;
	}
	
	std::vector<Gdiplus::Bitmap*> bitmapList;

	INT32 pixelsToOffset = 10;
	VisibleAreaCoordsStruct initialDims = getVisibleClientArea(hwnd);
	
	INT32 currentVisibleYOffset = initialDims.visibleAreaHeight;
	INT32 initialVisibleYOffset = currentVisibleYOffset;
	//scroll a few pixels up and get hbitmap, 
	//put into vector
	//merge by calling mergeBitmaps
	//scroll back
	if (scrollFullPageHeight > currentVisibleYOffset) {
		while (scrollFullPageHeight > currentVisibleYOffset) {
			ScrollWindow(hwnd, 0, -pixelsToOffset, NULL, NULL);
			UpdateWindow(hwnd);
			VisibleAreaCoordsStruct a = getVisibleClientArea(hwnd);
			Gdiplus::Bitmap* currBitmap = getBitmapFromAreaCoords(hwnd, a);
			bitmapList.push_back(currBitmap);
			currentVisibleYOffset += pixelsToOffset;
			Sleep(200);
		}
		ScrollWindow(hwnd, 0, currentVisibleYOffset - initialVisibleYOffset, NULL, NULL);
		UpdateWindow(hwnd);
	}
	else {
		VisibleAreaCoordsStruct a = getVisibleClientArea(hwnd);
		Gdiplus::Bitmap* currBitmap = getBitmapFromAreaCoords(hwnd, a);
		bitmapList.push_back(currBitmap);
	}
	//currentVisibleYOffset = adjusted image size;
	Gdiplus::Bitmap* gdiPlusMainImgBitmap = NULL;
	if (bitmapList.size() > 1) {
		gdiPlusMainImgBitmap = mergeBitmaps(bitmapList, currentVisibleYOffset);
	}
	else {
		gdiPlusMainImgBitmap = bitmapList.at(0);
	}

	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));
	if (scrSaveType == SCR_SAVETYPE::INTERNET) {
		_tcscpy(fullSavePath, sysInfoConfigDirectoryPath);
		TCHAR uuidString[256];
		_tcscat(fullSavePath, L"\\");
		_tcscat(fullSavePath, L"_temp.png");
	} else if (scrSaveType == SCR_SAVETYPE::LOCAL) {
		if (openFileDiag(hwnd, FILE_EXTENSION::PNG, fullSavePath, 1) == ACTION::CANCELED_OUT) {
			return ACTION::CANCELED_OUT;
		}
	}
	if (!gdiPlusMainImgBitmap->Save(fullSavePath, &pngEncoderCLSID, NULL) == Gdiplus::Status::Ok) {
		return ACTION::__ERROR;
	}

	//shutdown GDI+
	Gdiplus::GdiplusShutdown(gdiPlusToken);
	if (scrSaveType == SCR_SAVETYPE::LOCAL) {
		res->src.assign(fullSavePath);
	}
	if (scrSaveType == SCR_SAVETYPE::INTERNET) {
		RESULT_STRUCT resStruct = {};
		if (!uploadImage(&resStruct, fullSavePath)) {
			GenericMessageOK()
				.withMessage(L"Failed to upload")
				->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
				->display();
			return ACTION::__ERROR;
		}
		else {
			_tcscpy(uploadSrcLinkData.link, resStruct.__src);
		}
		DeleteFile(fullSavePath);
	}
	return ACTION::ACCEPTED;
}

VisibleAreaCoordsStruct getVisibleClientArea(HWND hwnd) {
	RECT winSize;
	GetClientRect(hwnd, &winSize);

	//visible client area
	INT32 areaWidth = winSize.right - winSize.left;
	INT32 areaHeight = winSize.bottom - winSize.top;
	return { areaWidth, areaHeight };
}

Gdiplus::Bitmap* getBitmapFromAreaCoords(HWND hwnd, VisibleAreaCoordsStruct &coords) {
	HDC mainWindowDC = GetDC(hwnd);
	HDC screenCapture = CreateCompatibleDC(mainWindowDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(mainWindowDC,
		coords.visibleAreaWidth, coords.visibleAreaHeight);
	SelectObject(screenCapture, hCaptureBitmap);
	BitBlt(screenCapture, 
		0, 
		0, 
		coords.visibleAreaWidth, 
		coords.visibleAreaHeight,
		mainWindowDC, 
		0, 
		0, 
		SRCCOPY | CAPTUREBLT);

	Gdiplus::Bitmap *gdiPlusMainImgBitmap = Gdiplus::Bitmap::FromHBITMAP(hCaptureBitmap, NULL);
	DeleteObject(hCaptureBitmap);
	ReleaseDC(hwnd, mainWindowDC);
	DeleteDC(screenCapture);
	return gdiPlusMainImgBitmap;
}

Gdiplus::Bitmap *mergeBitmaps(
	std::vector<Gdiplus::Bitmap*> bitmapsToMerge,
	INT32 adjustedImageSizeByYOffset) {
	bitmapsToMerge.at(0)->GetWidth();
	Gdiplus::Bitmap *completeBitmap = 
	new Gdiplus::Bitmap(
			bitmapsToMerge.at(0)->GetWidth(),
			adjustedImageSizeByYOffset);
	Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(completeBitmap);
	//iterate over the vector and draw images one by one
	INT32 offsetForLength = 10;
	for (auto bitmapIterator = bitmapsToMerge.begin();
		bitmapIterator != bitmapsToMerge.end();
		bitmapIterator++) {
		if (bitmapIterator == bitmapsToMerge.begin()) {
			g->DrawImage((*bitmapIterator), 0, 0);
		}
		else {
			g->DrawImage((*bitmapIterator), 0, offsetForLength);
			offsetForLength += 10;
		}
	}
	return completeBitmap;
}



