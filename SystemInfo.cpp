#include "SystemInfo.h"
wstring SystemInfo::getCPU()
{
	return (*this).CPU;
}
wstring SystemInfo::getRAM()
{
	return (*this).RAM;
}
void SystemInfo::setMB(wstring MB)
{
	(*this).MB = MB;
}
wstring SystemInfo::getMB()
{
	return (*this).MB = MB;
}
vector<wstring> SystemInfo::getGPUDevices()
{
	return (*this).GPUDevices;
}
vector<wstring> SystemInfo::getStorageMediums()
{
	return (*this).storageMediums;
}
vector<wstring> SystemInfo::getDisplayDevices()
{
	return (*this).displayDevices;
}
void SystemInfo::addDisplayDevice(wstring device)
{
	(*this).displayDevices.push_back(device);
}
void SystemInfo::addStorageMedium(wstring medium)
{
	(*this).storageMediums.push_back(medium);
}
void SystemInfo::addGPUDevice(wstring device)
{
	(*this).GPUDevices.push_back(device);
}
wstring SystemInfo::getOS()
{
	return (*this).OS;
}
void SystemInfo::setCPU(wstring CPU)
{	
	(*this).CPU = CPU;
}
void SystemInfo::setRAM(wstring RAM)
{
	(*this).RAM = RAM;
}
void SystemInfo::setOS(wstring OS)
{
	(*this).OS = OS;
}
vector<wstring> SystemInfo::getCDROMDevices(void)
{
	return (*this).CDROMDevices;
}
void SystemInfo::addCDROMDevice(wstring CDROM)
{
	(*this).CDROMDevices.push_back(CDROM);
}
void SystemInfo::setAudio(wstring audio)
{
	(*this).audio = audio;
}
wstring SystemInfo::getAudio(void)
{
	return (*this).audio;
}
wstring SystemInfo::getUptime(void)
{
	return (*this).uptime;
}
void SystemInfo::setUptime(wstring uptime)
{
	(*this).uptime = uptime;
}
