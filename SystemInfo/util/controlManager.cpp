#include "controlManager.h"
#include "utility.h"


const wstring ControlManager::UI_messagesTxt[] = {
		   L"Successfully written to ",
		   L"Error writing to ",
		   L"Link successfully copied to clipboard",
		   L"There's been a problem copying link to clipboard"
};

const unordered_map <ControlManager::UI_MESS_RES_ICON, wstring> ControlManager::messageCodeCaptionsMap =
{
	{ControlManager::UI_MESS_RES_ICON::SUCCESS, L"Success"},
	{ControlManager::UI_MESS_RES_ICON::QUESTION, L"User action required"},
	{ControlManager::UI_MESS_RES_ICON::WARN, L"Warning"},
	{ControlManager::UI_MESS_RES_ICON::FAILURE, L"Error" }
};

const unordered_map <ControlManager::UI_MESS_EXPORT_ACTION, vector<wstring>> ControlManager::UIMessageExportFileExtByUserExportModeCommandMap =
{
	{ControlManager::UI_MESS_EXPORT_ACTION::WRITE_OUT_HTML,
		{L".html", L".html files (*.html)\0*.html\0"}},
	{ControlManager::UI_MESS_EXPORT_ACTION::WRITE_OUT_IMG,
		{L".png", L".png images (*.png)\0*.png\0"}},
	{ControlManager::UI_MESS_EXPORT_ACTION::WRITE_OUT_TXT,
		{L".txt", L".txt files (*.txt)\0*.txt\0"}},
	{ControlManager::UI_MESS_EXPORT_ACTION::WRITE_OUT_XML,
		{L".xml", L".xml files (*.xml)\0*.xml\0"}}
};

HWND ControlManager::appCreateControl(
	TCHAR* className,
	int xOffset,
	int yOffSet,
	int width,
	int height,
	std::bitset<32> windowStyles,
	HWND parent,
	int controlMenuID,
	TCHAR* windowName,
	std::bitset<32> exStyle,
	HINSTANCE hInstance,
	LPVOID lParam
) {
	HWND createdWindow = CreateWindowEx(
		exStyle.to_ulong(),
		className,
		windowName,
		windowStyles.to_ulong(),
		xOffset,
		yOffSet,
		width,
		height,
		parent,
		(HMENU)controlMenuID,
		hInstance,
		lParam
	);
	if (!createdWindow) {
		DWORD errCode = GetLastError();
		GenericMessageOK()
			.withMessage(getSystemErrorCodeMessageForErrorCode(errCode))
			->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
			->display();
	}
	return createdWindow;
}

bool ControlManager::searchAndDestroy(
	HWND parentWindowHandle,
	TCHAR* className,
	TCHAR* windowName) {
	HWND foundWindowHandle = NULL;
	if ((foundWindowHandle = FindWindowEx(NULL, NULL, className, windowName)) != NULL) {
		return DestroyWindow(foundWindowHandle);
	}
	return false;
}