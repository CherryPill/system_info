#include "SystemInfo.h"
string SystemInfo::getCPU()
{
	return (*this).CPU;
}
string SystemInfo::getRAM()
{
	return (*this).RAM;
}
void SystemInfo::setMB(string MB)
{
	(*this).MB = MB;
}
string SystemInfo::getMB()
{
	return (*this).MB = MB;
}
vector<string> SystemInfo::getGPUDevices()
{
	return (*this).GPUDevices;
}
vector<string> SystemInfo::getStorageMediums()
{
	return (*this).storageMediums;
}
vector<string> SystemInfo::getDisplayDevices()
{
	return (*this).displayDevices;
}
void SystemInfo::addDisplayDevice(string device)
{
	(*this).displayDevices.push_back(device);
}
void SystemInfo::addStorageMedium(string medium)
{
	(*this).storageMediums.push_back(medium);
}
void SystemInfo::addGPUDevice(string device)
{
	(*this).GPUDevices.push_back(device);
}
string SystemInfo::getOS()
{
	return (*this).OS;
}
void SystemInfo::setCPU(string CPU)
{	
	(*this).CPU = CPU;
}
void SystemInfo::setRAM(string RAM)
{
	(*this).RAM = RAM;
}
void SystemInfo::setOS(string OS)
{
	(*this).OS = OS;
}
vector<string> SystemInfo::getCDROMDevices(void)
{
	return (*this).CDROMDevices;
}
void SystemInfo::addCDROMDevice(string CDROM)
{
	(*this).CDROMDevices.push_back(CDROM);
}
void SystemInfo::setAudio(string audio)
{
	(*this).audio = audio;
}
string SystemInfo::getAudio(void)
{
	return (*this).audio;
}
string SystemInfo::getUptime(void)
{
	return (*this).uptime;
}
void SystemInfo::setUptime(string uptime)
{
	(*this).uptime = uptime;
}
