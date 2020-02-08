#include <windows.h>
#include "core/SystemInfo.h"
#ifndef MAINWINDOWPROCEDURE_H
	#define MAINWINDOWPROCEDURE_H
	LRESULT CALLBACK mainWindowProc(HWND, UINT, WPARAM, LPARAM);
	BOOL CALLBACK SetFont(HWND child, LPARAM font);
	void createHardwareInfoHolders(HWND, SystemInfo*, int);
	void populateInfoHolders(SystemInfo *machine, HWND);
	void createIPToggleControl(HWND, int, int);
	void loadImages(void);
	void toggleIpAddress(HWND, SystemInfo*);
	void updateNetworkAdaptersView(SystemInfo*);
	unsigned int __stdcall updateUptime(void*);
	void scrollClientWindow(HWND hwnd, int bar, int pos);
	void fillGUI(HWND hwnd, SystemInfo *localMachine, int);
#endif