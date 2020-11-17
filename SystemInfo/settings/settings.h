#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <map>
#include <string>
#include "../glb/colorVals.h"
extern std::unordered_map<WPARAM, INT32> tabClickState;
extern std::unordered_map<WPARAM, WPARAM> checkBoxCheckedState;
class SettingsControl {
private:
	HWND controlHandle;
public:
	void setControlHandle(HWND controlHandle) {
		this->controlHandle = controlHandle;
	}
	HWND getControlHandle() {
		return controlHandle;
	}
};
class SettingsBlock {
private:
	std::vector<SettingsControl*> blockControls;
	HWND blockHandle;
public:

	std::vector<SettingsControl*>& getControls() {
		return blockControls;
	}
	void setControls(std::vector<SettingsControl*> blockControls) {
		this->blockControls = blockControls;
	}
};
class SettingsTab {
private:
	HWND tabContentWrapperHandle;
	std::vector<SettingsBlock*> tabBlocks;
public:
	void setTabContentWrapperHandle(HWND tabContentWrapperHandle) {
		this->tabContentWrapperHandle = tabContentWrapperHandle;
	}
	HWND getTabContentWrapperHandle() {
		return tabContentWrapperHandle;
	}
	void setTabBlocks(std::vector<SettingsBlock*> tabBlocks) {
		this->tabBlocks = tabBlocks;
	}
};

class SettingsWindow {
private:
	static const INT32 defaultWindowWidth = 500;
	static const INT32 defaultWindowHeight = 300;

	INT32 windowWidth = defaultWindowWidth;
	INT32 windowHeight = defaultWindowHeight;

	INT32 controlBtnsRequiredHeight = 100;

	INT32 effectiveWindowHeight = defaultWindowHeight - controlBtnsRequiredHeight;

	WNDPROC handlerProc;
	HWND windowHandle;
	//saved settings obj also needs to be here

	std::wstring exportInfoPreviewOsString;

	std::vector<SettingsTab*> tabs;
	std::vector<SettingsControl*> controlButtons;

public:
	void setHandlerProc(WNDPROC proc) {
		this->handlerProc = proc;
	}
	WNDPROC& getHandlerProcRef(WNDPROC proc) {
		return handlerProc;
	}
	INT32 getWindowWidth() {
		return windowWidth;
	}
	INT32 getWindowHeight() {
		return windowHeight;
	}
	void setWindowWidth(INT32 windowWidth) {
		this->windowWidth = windowWidth;
	}
	void setWindowHeight(INT32 windowHeight) {
		this->windowHeight = windowHeight;
	}
	void setEffectiveWindowHeight(INT32 effectiveWindowHeight) {
		this->effectiveWindowHeight = effectiveWindowHeight;
	}
	INT32 getEffectiveWindowHeight() {
		return effectiveWindowHeight;
	}

	void setWindowHandle(HWND windowHandle) {
		this->windowHandle = windowHandle;
	}
	HWND getWindowHandle() {
		return windowHandle;
	}

	void setTabs(std::vector<SettingsTab*> tabs) {
		this->tabs = tabs;
	}
	std::vector<SettingsTab*>& getTabs() {
		return tabs;
	}
	void setExportInfoPreviewOsString(std::wstring previewStr) {
		this->exportInfoPreviewOsString = previewStr;
	}

	std::wstring getExportInfoPreviewOsString() {
		return this->exportInfoPreviewOsString;
	}

	std::vector<SettingsControl*> getControlButtons() {
		return this->controlButtons;
	}
	
	void setControlButtons(std::vector<SettingsControl*> controlButtons) {
		this->controlButtons = controlButtons;
	}

	void showTabs() {
		//show depending on state of each tab
		for (std::vector<SettingsTab*>::iterator iterator = this->tabs.begin();
			iterator != this->tabs.end();
			iterator++) {
			INT32 ctrlId = GetDlgCtrlID((*iterator)->getTabContentWrapperHandle());
			HWND handle = (*iterator)->getTabContentWrapperHandle();
			ShowWindow((*iterator)->getTabContentWrapperHandle(),
				tabClickState.at(GetDlgCtrlID((*iterator)->getTabContentWrapperHandle())));
		}
	}
};

class SavedUserSettings {
private:

	typedef BOOL(SavedUserSettings::* chkBoxStateGetter) ();

	chkBoxStateGetter checkBoxStateGetters[5] =
	{ &SavedUserSettings::getShowCpuUsage,
		&SavedUserSettings::getShowHDDTemp,
		&SavedUserSettings::getScreenshotCaptureClientAreaOnly,
		&SavedUserSettings::getRememberLastWindowPosition,
		&SavedUserSettings::getHideIPAddress
	};
	
	//tab 0
	BOOL isShowCPUusage = TRUE;
	BOOL isScreenshotCaptureClientAreaOnly = TRUE;
	BOOL isRememberLastWindowPosition = FALSE;
	BOOL isShowHDDTemp = FALSE;
	BOOL isHideIPAddress = TRUE;

	//tab 1
	COLORREF htmlExportHeaderBgColorRGB = getColorFromPreset(COLOR::PURPLE);
	COLORREF htmlExportHeaderFgColorRGB = getColorFromPreset(COLOR::WHITE);
	COLORREF htmlExportInfoBgColorRGB = getColorFromPreset(COLOR::WHITE);
	COLORREF htmlExportInfoFgColorRGB = getColorFromPreset(COLOR::BLACK);


public:
	chkBoxStateGetter *getChkBoxGetters() {
		return this->checkBoxStateGetters;
	}
	BOOL getHideIPAddress() {
		return this->isHideIPAddress;
	}
	BOOL *getHideIPAddressRef() {
		return &isHideIPAddress;
	}
	void setHideIPAddress(BOOL isHideIPAddress) {
		this->isHideIPAddress = isHideIPAddress;
	}
	BOOL getShowCpuUsage() {
		return this->isShowCPUusage;
	}
	BOOL *getShowCpuUsageRef() {
		return &isShowCPUusage;
	}
	void setShowCpuUsage(BOOL isShowCPUusage) {
		this->isShowCPUusage = isShowCPUusage;
	}
	BOOL getShowHDDTemp() {
		return this->isShowHDDTemp;
	}
	BOOL *getShowHDDTempRef() {
		return &isShowHDDTemp;
	}
	void setShowHDDTemp(BOOL isShowHDDTemp) {
		this->isShowHDDTemp = isShowHDDTemp;
	}
	BOOL getScreenshotCaptureClientAreaOnly() {
		return this->isScreenshotCaptureClientAreaOnly;
	}
	BOOL *getScreenshotCaptureClientAreaOnlyRef() {
		return &isScreenshotCaptureClientAreaOnly;
	}
	void setScreenshotCaptureClientAreaOnly(BOOL isScreenshotCaptureClientAreaOnly) {
		this->isScreenshotCaptureClientAreaOnly = isScreenshotCaptureClientAreaOnly;
	}
	BOOL getRememberLastWindowPosition() {
		return this->isRememberLastWindowPosition;
	}
	BOOL *getRememberLastWindowPositionRef() {
		return &isRememberLastWindowPosition;
	}
	void setRememberLastWindowPosition(BOOL isRememberLastWindowPosition) {
		this->isRememberLastWindowPosition = isRememberLastWindowPosition;
	}
	COLORREF getHtmlExportHeaderBgColorRGB() {
		return this->htmlExportHeaderBgColorRGB;
	}
	COLORREF *getHtmlExportHeaderBgColorRGBRef() {
		return &htmlExportHeaderBgColorRGB;
	}
	void setHtmlExportHeaderBgColorRGB(COLORREF htmlExportHeaderBgColorRGB) {
		this->htmlExportHeaderBgColorRGB = htmlExportHeaderBgColorRGB;
	}
	COLORREF getHtmlExportHeaderFgColorRGB() {
		return this->htmlExportHeaderFgColorRGB;
	}
	COLORREF *getHtmlExportHeaderFgColorRGBRef() {
		return &htmlExportHeaderFgColorRGB;
	}
	void setHtmlExportHeaderFgColorRGB(COLORREF htmlExportHeaderFgColorRGB) {
		this->htmlExportHeaderFgColorRGB = htmlExportHeaderFgColorRGB;
	}
	COLORREF getHtmlExportInfoBgColorRGB() {
		return this->htmlExportInfoBgColorRGB;
	}
	COLORREF *getHtmlExportInfoBgColorRGBRef() {
		return &htmlExportInfoBgColorRGB;
	}
	void setHtmlExportInfoBgColorRGB(COLORREF htmlExportInfoBgColorRGB) {
		this->htmlExportInfoBgColorRGB = htmlExportInfoBgColorRGB;
	}
	COLORREF getHtmlExportInfoFgColorRGB() {
		return this->htmlExportInfoFgColorRGB;
	}
	COLORREF *getHtmlExportInfoFgColorRGBRef() {
		return &htmlExportInfoFgColorRGB;
	}
	void setHtmlExportInfoFgColorRGB(COLORREF htmlExportInfoFgColorRGB) {
		this->htmlExportInfoFgColorRGB = htmlExportInfoFgColorRGB;
	}
};


std::vector<SettingsTab*> createAndFillTabs(SettingsWindow*);
std::vector<SettingsControl*> createControlButtons(SettingsWindow*);

void registerSettingsDialogClass();
void registerTabContentWrapperWindowClass();
LRESULT CALLBACK settingsDialogProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK tabWrapperProc(HWND, UINT, WPARAM, LPARAM);

bool CALLBACK __SetFont(HWND child, LPARAM font);

HWND createTabContentWrapper(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent);
std::vector<SettingsBlock*> createControlsForTab(HWND controlTabWrapperHandle, INT32 id, SettingsWindow* sw);
void handleTabSelectionChange(INT32);
COLORREF initializeColorDlgBox(HWND hwnd);
HWND createGenericContainer(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent);
INT32 createControlGroupBox(HWND hwnd, std::vector<SettingsControl*> controls, INT32 yOffset);

//filled based on the number of tabs
