#pragma once
#include <Windows.h>
#include <gdiplus.h> 
#include <iostream>
#include <vector>
#include <string.h>
#include "../network/common/network.h"
#include "../const/appconst.h"
using namespace std;
class NetAdapter;
class SystemInfo {
	private:
	SystemInfo() {
		this->BIOS = L"BIOS not detected";
		this->OS = L"OS not detected";
		this->RAM = L"RAM not detected";
		this->MB = L"Motherboard not detected";
		this->CPU = L"CPU not detected";
		this->audio = L"Sound card not detected";
		this->uptime = L"Uptime not set";
	}
	wstring BIOS;
	wstring OS;
	wstring CPU;
	wstring MB;
	wstring RAM;
	vector<wstring> GPUDevices;
	vector<wstring> storageMediums;
	vector<wstring> displayDevices;
	vector<wstring> CDROMDevices;
	vector<NetAdapter> networkAdapters;
	vector<wstring> networkAdaptersText;
	wstring audio;
	wstring uptime;
	wstring snapshotGenDateTime;

	public:
	static SystemInfo *getCurrentInstance() {
		static SystemInfo *currentInstance = new SystemInfo();
		return currentInstance;
	}

	vector<wstring> getCDROMDevices(void);
	wstring getBIOS(void);
	wstring getUptime(void);
	wstring getCPU(void);
	wstring getRAM(void);
	wstring getMB(void);
	wstring getAudio(void);
	vector<wstring> getGPUDevices(void);
	vector<wstring> getStorageMediums(void);
	vector<wstring> getDisplayDevices(void);
	vector<NetAdapter> getNetworkAdapters(void);
	vector<wstring> getNetworkAdaptersText(void);
	vector<wstring>& getNetworkAdaptersTextRef(void) {
		return this->networkAdaptersText;
	}
	wstring getOS(void);
	wstring getSnapshotGenDateTime();
	void setBIOS(wstring bios);
	void setUptime(wstring uptime);
	void setCPU(wstring CPU);
	void setRAM(wstring RAM);
	void setMB(wstring MB);
	void addDisplayDevice(wstring device);
	void addStorageMedium(wstring medium);
	void addNetworkAdapter(NetAdapter adapter);
	void addNetworkAdapterText(wstring adapter);
	void addGPUDevice(wstring device);
	void setOS(wstring OS);
	void setAudio(wstring audio);
	void addCDROMDevice(wstring CDROM);
	void setSnapshotGenDateTime(wstring dt);
};
