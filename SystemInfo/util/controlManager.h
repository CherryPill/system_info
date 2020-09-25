#pragma once
#include <Windows.h>
#include <strsafe.h>
#include <bitset>
#include "../glb/globalVars.h"


//intermediate control object with null fields by default
class IntermediateControl {
private:
	DWORD dwExStyle = NULL;
	TCHAR* lpClassName = NULL;
	TCHAR* lpWindowName = NULL;
	DWORD dwStyle = NULL;
	INT32 X = 0;
	INT32 Y = 0;
	INT32 nWidth = 0;
	INT32 nHeight = 0;
	HWND hWndParent = NULL;
	INT32 hMenu = NULL;
	HINSTANCE hInstance = NULL;
	LPVOID lpParam = NULL;
public:
	void setDwExStyle(DWORD dwExStyle) {
		this->dwExStyle = dwExStyle;
	}
	DWORD getDwExStyle() {
		return this->dwExStyle;
	}
	void setDwStyle(DWORD dwStyle) {
		this->dwStyle = dwStyle;
	}
	DWORD getDwStyle() {
		return this->dwStyle;
	}
	void setlpClassName(TCHAR* lpClassName) {
		this->lpClassName = lpClassName;
	}
	TCHAR* getlpClassName() {
		return this->lpClassName;
	}
	void setlpWindowName(TCHAR* lpWindowName) {
		this->lpWindowName = lpWindowName;
	}
	TCHAR* getlpWindowName() {
		return this->lpWindowName;
	}
	void setX(INT32 X) {
		this->X = X;
	}
	INT32 getX() {
		return this->X;
	}
	void setY(INT32 Y) {
		this->Y = Y;
	}
	INT32 getY() {
		return this->Y;
	}
	void setnWidth(INT32 nWidth) {
		this->nWidth = nWidth;
	}
	INT32 getnWidth() {
		return this->nWidth;
	}
	void setnHeight(INT32 nHeight) {
		this->nHeight = nHeight;
	}
	INT32 getnHeight() {
		return this->nHeight;
	}
	void sethWndParent(HWND hWndParent) {
		this->hWndParent = hWndParent;
	}
	HWND gethWndParent() {
		return this->hWndParent;
	}
	void sethMenu(INT32 hMenu) {
		this->hMenu = hMenu;
	}
	INT32 gethMenu() {
		return this->hMenu;
	}
	void sethInstance(HINSTANCE hInstance) {
		this->hInstance = hInstance;
	}
	HINSTANCE gethInstance() {
		return this->hInstance;
	}
	void setlpParam(LPVOID lpParam) {
		this->lpParam = lpParam;
	}
	LPVOID getlpParam() {
		return this->lpParam;
	}
};


class ControlBuilder {
private:
	IntermediateControl* interMediateControl;
	ControlBuilder() {
		this->interMediateControl = new IntermediateControl();
	}
public:
	static ControlBuilder* initBuilder() {
		return new ControlBuilder();
	}

	ControlBuilder* withClassName(TCHAR* lpClassName) {
		this->interMediateControl->setlpClassName(lpClassName);
		return this;
	}
	ControlBuilder* withDimensions(INT32 w, INT32 h) {
		this->interMediateControl->setnWidth(w);
		this->interMediateControl->setnHeight(h);
		return this;
	}
	ControlBuilder* withCoords(INT32 x, INT32 y) {
		this->interMediateControl->setX(x);
		this->interMediateControl->setY(y);
		return this;
	}
	ControlBuilder* withControlId(INT32 id) {
		this->interMediateControl->sethMenu(id);
		return this;
	}
	ControlBuilder* withStyles(DWORD styles) {
		this->interMediateControl->setDwStyle(styles);
		return this;
	}
	ControlBuilder* withExStyles(DWORD exStyles) {
		this->interMediateControl->setDwExStyle(exStyles);
		return this;
	}
	ControlBuilder* withWindowName(TCHAR* windowName) {
		this->interMediateControl->setlpWindowName(windowName);
		return this;
	}
	ControlBuilder* withParent(HWND parent) {
		this->interMediateControl->sethWndParent(parent);
		return this;
	}
	ControlBuilder* withInstance(HINSTANCE hInstance) {
		this->interMediateControl->sethInstance(hInstance);
		return this;
	}
	ControlBuilder* withParam(LPVOID lParam) {
		this->interMediateControl->setlpParam(lParam);
		return this;
	}

	HWND build() {
		return CreateWindowEx(
			this->interMediateControl->getDwExStyle(),
			this->interMediateControl->getlpClassName(),
			this->interMediateControl->getlpWindowName(),
			this->interMediateControl->getDwStyle(),
			this->interMediateControl->getX(),
			this->interMediateControl->getY(),
			this->interMediateControl->getnWidth(),
			this->interMediateControl->getnHeight(),
			this->interMediateControl->gethWndParent(),
			(HMENU)this->interMediateControl->gethMenu(),
			this->interMediateControl->gethInstance(),
			this->interMediateControl->getlpParam()
		);
	}
};
class ControlManager {
	public:

		const static wstring UI_messagesTxt[];

		enum class UI_MESS_RES_ICON {
			SUCCESS = MB_ICONINFORMATION, //info
			FAILURE = MB_ICONERROR, //error
			WARN = MB_ICONWARNING,
			QUESTION = MB_ICONQUESTION
		};

		enum class UI_MESS_EXPORT_ACTION {
			WRITE_OUT_TXT = ID_EXPORT_TXT,
			WRITE_OUT_XML = ID_EXPORT_XML,
			WRITE_OUT_HTML = ID_EXPORT_HTML,
			WRITE_OUT_IMG = ID_FILE_TAKESCREENSHOT_SAVE_LOCALLY
		};

		enum class UI_MESS_TYPE {
			GENERIC,
			EXPORT
		};

		const static unordered_map <UI_MESS_RES_ICON, wstring> messageCodeCaptionsMap;

		const static unordered_map <UI_MESS_EXPORT_ACTION, vector<wstring>> UIMessageExportFileExtByUserExportModeCommandMap;

		static HWND appCreateControl(
			TCHAR* className,
			int xOffset,
			int yOffSet,
			int width,
			int height,
			std::bitset<32> windowStyles,
			HWND parent,
			int controlMenuID,
			TCHAR* windowName = NULL,
			std::bitset<32> exStyle = 0,
			HINSTANCE hInstance = NULL,
			LPVOID lParam = NULL);

		static bool searchAndDestroy(
			HWND parentWindowHandle, 
			TCHAR *className,
			TCHAR *windowName);
};

class AbstractMessage {
	protected:
		wstring message;
		ControlManager::UI_MESS_RES_ICON icon;
	public: 
		virtual UINT display() = 0;
		AbstractMessage *withMessage(const wstring message) {
			this->message.assign(message);
			return this;
		}
		AbstractMessage* withIcon(ControlManager::UI_MESS_RES_ICON icon) {
			this->icon = icon;
			return this;
		}
};

class GenericMessageOK : public AbstractMessage {
	public:
		UINT display() {
			return MessageBox(
				mainWindowHwnd,
				this->message.c_str(),
				ControlManager::messageCodeCaptionsMap.at(this->icon).c_str(),
				MB_OK | static_cast<int>(this->icon));
		}
};

class PromptMessageYesNo : public AbstractMessage {
	public:
		UINT display() {
			return MessageBox(
				NULL,
				this->message.c_str(),
				ControlManager::messageCodeCaptionsMap.at(this->icon).c_str(),
				MB_YESNO | static_cast<int>(this->icon));
		}
};