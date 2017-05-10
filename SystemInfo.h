#pragma once
#include <iostream>
#include <vector>
#include <string.h>
#include "appconst.h"
using namespace std;
class SystemInfo
{
private:

	SystemInfo()
	{
			this->OS = "OS not detected";
			this->RAM = "Motherboard not detected";
			this->audio = "Sound card not detected";
			this->uptime = "Uptime not set";

			//_tcscpy(this->OS, "OS not detected");
			//_tcscpy(this->RAM, "RAM not detected");
			//_tcscpy(this->MB, "Motherboard not detected");
			//_tcscpy(this->audio, "Sound card not detected");
			//_tcscpy(this->uptime, "Uptime not set");
	}
	string OS;
	string CPU;
	string MB;
	string RAM;
	vector<string> GPUDevices;
	vector<string> storageMediums;
	vector<string> displayDevices;
	vector<string> CDROMDevices;
	string audio;
	string uptime;
public:
	static SystemInfo *getCurrentInstance()
	{
		static SystemInfo *currentInstance = new SystemInfo();
		return currentInstance;
	}
	vector<string> getCDROMDevices(void);
	string  getUptime(void);
	string  getCPU(void);
	string  getRAM(void);
	string  getMB(void);
	string  getAudio(void);
	vector<string> getGPUDevices(void);
	vector<string> getStorageMediums(void);
	vector<string> getDisplayDevices(void);
	string getOS(void);
	void setUptime(string uptime);
	void setCPU(string CPU);
	void setRAM(string RAM);
	void setMB(string MB);
	void addDisplayDevice(string device);
	void addStorageMedium(string medium);
	void addGPUDevice(string device);
	void setOS(string OS);
	void setAudio(string audio);
	void addCDROMDevice(string CDROM);
};

