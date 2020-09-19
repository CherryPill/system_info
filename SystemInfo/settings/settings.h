#pragma once
#include <Windows.h>
#include <CommCtrl.h>
extern std::unordered_map<WPARAM, INT32> tabClickState;
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
	static const INT32 defaultWindowWidth = 450;
	static const INT32 defaultWindowHeight = 300;

	INT32 windowWidth = defaultWindowWidth;
	INT32 windowHeight = defaultWindowHeight;

	WNDPROC handlerProc;
	HWND windowHandle;

	std::vector<SettingsTab*> tabs;

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

	//tab 0
	BOOL isShowCPUusage;
	BOOL isScreenshotCaptureClientAreaOnly;
	BOOL isRememberLastWindowPosition;

	//tab 1
	COLORREF htmlExportHeaderBgColorRGB;
	COLORREF htmlExportHeaderFgColorRGB;
	COLORREF htmlExportInfoBgColorRGB;
	COLORREF htmlExportInfoFgColorRGB;



};


std::vector<SettingsTab*> createAndFillTabs(SettingsWindow*);
void registerSettingsDialogClass();
void registerTabContentWrapperWindowClass();
LRESULT CALLBACK settingsDialogProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK tabWrapperProc(HWND, UINT, WPARAM, LPARAM);
bool CALLBACK __SetFont(HWND child, LPARAM font);

HWND createTabContentWrapper(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent);
std::vector<SettingsBlock*> createControlsForTab(HWND controlTabWrapperHandle, INT32 id);
void handleTabSelectionChange(INT32);
COLORREF initializeColorDlgBox(HWND hwnd);
HWND createGenericContainer(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent);
//filled based on the number of tabs
