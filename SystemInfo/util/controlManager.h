#pragma once
#include <Windows.h>
#include <strsafe.h>
#include <bitset>
#include "../glb/globalVars.h"
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
			DWORD exStyle = 0,
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