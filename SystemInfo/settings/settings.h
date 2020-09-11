#pragma once
#include <Windows.h>
#include <CommCtrl.h>

extern std::unordered_map<WPARAM, BOOL> tabClickState;
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

	std::vector<SettingsControl*> &getControls() {
		return blockControls;
	}
	void setControls(std::vector<SettingsControl*> blockControls) {
		this->blockControls = blockControls;
	}
};
class SettingsTab {
private:
	HWND handle;
	HWND tabContentWrapperHandle;
	std::vector<SettingsBlock*> tabBlocks;
public:
	void setHandle(HWND hwnd) {
		this->handle = hwnd;
	}
	HWND getHandle() {
		return handle;
	}
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
			ShowWindow((*iterator)->getTabContentWrapperHandle(),
				tabClickState.at(GetDlgCtrlID((*iterator)->getTabContentWrapperHandle())));
		}
	}
};


std::vector<SettingsTab*> createAndFillTabs();
void registerSettingsDialogClass();
void registerTabContentWrapperWindowClass();
LRESULT CALLBACK settingsDialogProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK tabWrapperProc(HWND, UINT, WPARAM, LPARAM);
bool CALLBACK __SetFont(HWND child, LPARAM font);

HWND createTabContentWrapper(INT32);
std::vector<SettingsBlock*> createControlsForTab(HWND controlTabWrapperHandle, INT32 id);
void handleTabSelectionChange(INT32);
//filled based on the number of tabs
