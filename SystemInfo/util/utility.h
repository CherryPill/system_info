#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <fstream>
#include <tchar.h>
#include "../core/SystemInfo.h"
#include "../glb/globalVars.h"
#include "../glb/colorVals.h"

#include "../util/controlManager.h"
#include "../settings/settings.h"

enum APPLICATION_OPERATION_MODE {
	PARENT_NON_ADMIN,
	PARENT_ADMIN,
	SNAPSHOT
};
enum class FILE_IO_OPERATION {
	SAVE_AS,
	OPEN
};

static wstring ipToggleText[]{
	L"Hide IP",
	L"Show IP"
};

static wstring writeOutPrefix[]{
	L"",
	L"",
	L"",
	L"<subitem>"
};

static wstring writeOutPostfix[]{
	L"\n",
	L"\n\t",
	L"<br />",
	L"</subitem>"
};

enum class WRITE_OUT_TYPE {
	APP_WINDOW,
	FILE_TXT,
	FILE_HTML,
	FILE_XML
};

enum class OS_INFO_TYPES {
	MIDDLEWARE_BIOS = 1,
	SOFTWARE_OS,
	HARDWARE_MB,
	HARDWARE_RAM,
	HARDWARE_VIDEO_ADAPTER,
	HARDWARE_DISPLAY,
	HARDWARE_STORAGE,
	HARDWARE_CDROM,
	HARDWARE_NETWORK
};

enum class WINDOW_CENTER_TYPE {
	CENTER_MAIN,
	CENTER_CHILD
};

enum class FILE_EXTENSION {
	TXT,
	XML,
	HTML,
	PNG
};

enum class SCR_SAVETYPE {
	LOCAL,
	INTERNET
};
static std::wstring storageMediumManufacturers[20]{
	L"Western Digital",
	L"Seagate",
	L"Fujitsu",
	L"Hitachi",
	L"Samsung",
	L"IMB"
};

enum class ENCODER_IMG_TYPES {
	IMG_PNG = 0,
	IMG_BMP,
	IMG_JPG,
	IMG_GIF,
	IMG_TIFF
};

const 
static 
unordered_map <ENCODER_IMG_TYPES, wstring>
	encoderImgTypesToStringMap = {
		{ENCODER_IMG_TYPES::IMG_PNG, L"image/png"},
		{ENCODER_IMG_TYPES::IMG_BMP, L"image/bmp"},
		{ENCODER_IMG_TYPES::IMG_JPG, L"image/jpeg"},
		{ENCODER_IMG_TYPES::IMG_GIF, L"image/gif"},
		{ENCODER_IMG_TYPES::IMG_TIFF, L"image/tiff"}
	};

static std::wstring imageEncodersForGdiPlus[]{
	L"image / bmp",
	L"image / jpeg",
	L"image / gif",
	L"image / tiff",
	L"image / png"
};

typedef struct RESULT_STRUCT {
	ACTION result;
	std::wstring src;
	TCHAR __src[256];
};

static std::wstring actionPromptText[]{
	L"Open exported file?"
};

void centerWindow(POINT*);
void positionWindow(POINT*, INT32, INT32);
std::string& BstrToStdString(const BSTR, std::string&, int cp = CP_UTF8);
void trimNullTerminator(std::wstring&);
std::wstring parseDiskStorageName(std::wstring);
std::wstring convertUIntToString(UINT64);
void trimWhiteSpace(std::wstring&);
void generateFileName(TCHAR*, FILE_EXTENSION);
UINT32 getInfoBoxItemCount(UINT32, SystemInfo*);
UINT32 adjustItemHeight(HWND, UINT32, UINT32);
void getCurrentDateTime(TCHAR *);
void getCurrentDateTimeVerbose(TCHAR *);
std::wstring formListString(SystemInfo*, OS_INFO_TYPES, WRITE_OUT_TYPE);
enum class ACTION openFileDiag(HWND, FILE_EXTENSION, TCHAR*, int);
void writeToFile(wofstream&, SystemInfo*, int, WRITE_OUT_TYPE);
wstring fromChToWideStr(char *value);
wstring fromIntToWideStr(int);
std::wstring convertStringToWide(const std::string& as);
void prependZeroToStr(WORD min, TCHAR *minBuff);
vector<wstring> stringSplit(const wchar_t *s, wchar_t delimiter);
std::wstring netAdapterStringWrapper(NetAdapter);
void getFileNameFromPath(TCHAR *fullPath, TCHAR *fileName);
ACTION fileIOCheck(wofstream&);
void calculateTimeAndFormat(TCHAR*);
BOOL openDefAppForExpData(WORD command, RESULT_STRUCT *res);
void configAppData();
bool dirExists(LPCTSTR);
std::wstring convertWmiCapacityToGB(std::wstring);
void removeTabulation(std::wstring&);
void condenseSpaces(std::wstring&);
std::wstring getSystemErrorCodeMessageForErrorCode(DWORD);
std::wstring formMessageForUIExportByExportAction(ControlManager::UI_MESS_RES_ICON res, DWORD act);
int GetEncoderClsid(const TCHAR *format, CLSID *pClsid);
TCHAR* convertColorReftoHexColorString(COLORREF);
UINT parseBool(BOOL);
BOOL parseCheckBoxState(UINT);
SIZE *getAdjustedDimensionsForStaticCntrl(HWND hwnd, TCHAR *string);
void resizeWindow(HWND hwndParent, HWND cntrlHwnd, INT32 newWidth);
void resizeWindow(HWND hwndParent, HWND cntrlHwnd, INT32 newWidth, INT32 newHeight);
COLORREF getColorForNumberGauge(INT32);
void updateWindow(HWND, INT32);
class SavedUserSettingsHelper {
private:
	//path for user settings is %current_user_name%/AppData/<configFileName>
	static TCHAR* configFileName;
	static TCHAR* fullConfigFilePath;
	
public:
	static void initializeFullConfigFilePath() {
		if (!sysInfoConfigDirectoryPath) {
			configAppData();
		}
		fullConfigFilePath = new TCHAR[256]{ 0 };
		_tcscat(fullConfigFilePath, sysInfoConfigDirectoryPath);
		_tcscat(fullConfigFilePath, L"\\");
		_tcscat(fullConfigFilePath, SavedUserSettingsHelper::configFileName);
	}
	static BOOL saveSettingsToDisk(SavedUserSettings* settings) {
		//sysInfoConfigDirectoryPath
		std::ofstream configFile;
		configFile.std::ofstream::open(SavedUserSettingsHelper::fullConfigFilePath, 
			std::ios::binary | std::ios::out);
		if (configFile.good()) {
			configFile.write(reinterpret_cast<const char*>((settings->getShowCpuUsageRef())), sizeof(BOOL));
			configFile.write(reinterpret_cast<const char*>(settings->getShowHDDTempRef()), sizeof(BOOL));
			configFile.write(reinterpret_cast<const char*>(settings->getRememberLastWindowPositionRef()), sizeof(BOOL));
			configFile.write(reinterpret_cast<const char*>(settings->getScreenshotCaptureClientAreaOnlyRef()), sizeof(BOOL));
			configFile.write(reinterpret_cast<const char*>(settings->getHideIPAddressRef()), sizeof(BOOL));
			
			configFile.write(reinterpret_cast<const char*>(settings->getHtmlExportHeaderBgColorRGBRef()), sizeof(COLORREF));
			configFile.write(reinterpret_cast<const char*>(settings->getHtmlExportHeaderFgColorRGBRef()), sizeof(COLORREF));
			configFile.write(reinterpret_cast<const char*>(settings->getHtmlExportInfoBgColorRGBRef()), sizeof(COLORREF));
			configFile.write(reinterpret_cast<const char*>(settings->getHtmlExportInfoFgColorRGBRef()), sizeof(COLORREF));

			configFile.std::ofstream::close();
			return TRUE;
		}
		return FALSE;
	}
	static BOOL loadSettingsFromDisk(SavedUserSettings *settings) {
		std::ifstream configFile;
		
		configFile.std::ifstream::open(SavedUserSettingsHelper::fullConfigFilePath, std::ios::binary
			| std::ios::in);
		if (configFile.good()) {
			configFile.read((char*)settings->getShowCpuUsageRef(), sizeof(BOOL));
			configFile.read((char*)settings->getShowHDDTempRef(), sizeof(BOOL));
			configFile.read((char*)settings->getRememberLastWindowPositionRef(), sizeof(BOOL));
			configFile.read((char*)settings->getScreenshotCaptureClientAreaOnlyRef(), sizeof(BOOL));
			configFile.read((char*)(settings->getHideIPAddressRef()), sizeof(BOOL));

			configFile.read((char*)(settings->getHtmlExportHeaderBgColorRGBRef()), sizeof(COLORREF));
			configFile.read((char*)(settings->getHtmlExportHeaderFgColorRGBRef()), sizeof(COLORREF));
			configFile.read((char*)(settings->getHtmlExportInfoBgColorRGBRef()), sizeof(COLORREF));
			configFile.read((char*)(settings->getHtmlExportInfoFgColorRGBRef()), sizeof(COLORREF));

			configFile.std::ifstream::close();
			return TRUE;
		}
		return FALSE;
	}
	static void fillSettingsCheckBoxState(SavedUserSettings* settings) {
		//explanation:
		//SavedUserSettings::chkBoxStateGetter func = settings->checkBoxStateGetters[0];
		//(settings->*c)();

		for (int x = 0; x < 5; x++) {
			BOOL res = (settings->*settings->getChkBoxGetters()[x])();
			checkBoxCheckedState.insert_or_assign(
				SETTINGS_WINDOW_CHKBOX_IDS[x], 
				parseBool((settings->*settings->getChkBoxGetters()[x])()));
		}
	}
	BOOL setDefaultSettings() {
		//isShowCPUusage = isScreenshotCaptureClientAreaOnly = TRUE;
		//isRememberLastWindowPosition = FALSE;
		//htmlExportHeaderBgColorRGB = COLOR_PURPLE; //purple in rgb
		//htmlExportHeaderFgColorRGB = COLOR_WHITE;
		//htmlExportInfoBgColorRGB = COLOR_WHITE;
		//htmlExportInfoFgColorRGB = COLOR_BLACK;
		//saveSettingsToDisk();
	}
	static void readUISettingsState(HWND enclosingTabWrapperHandle) {
		UINT res = IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_CPU_USAGE);
		glbUserSettings->setShowCpuUsage(parseCheckBoxState(IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_CPU_USAGE)));
		glbUserSettings->setShowHDDTemp(parseCheckBoxState(IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_HDD_TEMP)));
		glbUserSettings->setScreenshotCaptureClientAreaOnly(parseCheckBoxState(IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_SCRCAP_CLIENT_ONLY)));
		glbUserSettings->setRememberLastWindowPosition(parseCheckBoxState(IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_RMB_LAST_WIN_POS)));
		glbUserSettings->setHideIPAddress(parseCheckBoxState(IsDlgButtonChecked(enclosingTabWrapperHandle, TAB_CONTENT_CHKBOX_HIDE_IP_ADDR)));
		SavedUserSettingsHelper::fillSettingsCheckBoxState(glbUserSettings);
		if (SavedUserSettingsHelper::saveSettingsToDisk(glbUserSettings)) {
			GenericMessageOK()
				.withMessage(L"Settings saved")
				->withIcon(ControlManager::UI_MESS_RES_ICON::SUCCESS)
				->top()
				->display();
		}
		else {
			GenericMessageOK()
				.withMessage(L"Error occurred while saving your settings")
				->withIcon(ControlManager::UI_MESS_RES_ICON::FAILURE)
				->top()
				->display();
		}
		//use IsDlgButtonChecked and set glbusersettings to appropriate value
		//also fix html export theme first so that colors actually change on ui
	}

};
class ColorUtil {
	private:
		static const INT32 DARK_COLOR_UPPER_BOUND = 80;
	public:
		static BOOL isColorTooDark(COLORREF color) {
			if (GetRValue(color) < DARK_COLOR_UPPER_BOUND && 
				GetBValue(color) < DARK_COLOR_UPPER_BOUND &&
				GetGValue(color) < DARK_COLOR_UPPER_BOUND) {
				return TRUE;
			}
			return FALSE;
		}
		static BOOL isColorTooDark(COLOR color) {
			return isColorTooDark(colorPresetsMap.at(color));
		}
		static COLORREF pickAppropriateFgSysColor(COLORREF bgColor) {
			if (isColorTooDark(bgColor)) {
				return colorPresetsMap.at(COLOR::WHITE);
			}
			else {
				return colorPresetsMap.at(COLOR::BLACK);
			}
		}
};
#endif