#include "SystemInfo.h"
wstring SystemInfo::getCPU() {
	return (*this).CPU;
}

wstring SystemInfo::getRAM() {
	return (*this).RAM;
}

void SystemInfo::setMB(wstring MB) {
	(*this).MB = MB;
}

wstring SystemInfo::getMB() {
	return (*this).MB;
}

vector<wstring> SystemInfo::getGPUDevices() {
	return (*this).GPUDevices;
}

vector<wstring> SystemInfo::getStorageMediums() {
	return (*this).storageMediums;
}

vector<wstring> SystemInfo::getDisplayDevices() {
	return (*this).displayDevices;
}

void SystemInfo::addDisplayDevice(wstring device) {
	(*this).displayDevices.push_back(device);
}

void SystemInfo::addStorageMedium(wstring medium) {
	(*this).storageMediums.push_back(medium);
}

void SystemInfo::addNetworkAdapter(NetAdapter adapter) {
	(*this).networkAdapters.push_back(adapter);
}

void SystemInfo::addNetworkAdapterText(wstring adapter) {
	(*this).networkAdaptersText.push_back(adapter);
}

vector<wstring> SystemInfo::getNetworkAdaptersText() {
	return (*this).networkAdaptersText;
}

void SystemInfo::addGPUDevice(wstring device) {
	(*this).GPUDevices.push_back(device);
}

wstring SystemInfo::getOS() {
	return (*this).OS;
}

void SystemInfo::setCPU(wstring CPU) {	
	(*this).CPU = CPU;
}

void SystemInfo::setRAM(wstring RAM) {
	(*this).RAM = RAM;
}

void SystemInfo::setOS(wstring OS) {
	(*this).OS = OS;
}

vector<wstring> SystemInfo::getCDROMDevices(void) {
	return (*this).CDROMDevices;
}

void SystemInfo::addCDROMDevice(wstring CDROM) {
	(*this).CDROMDevices.push_back(CDROM);
}

void SystemInfo::setAudio(wstring audio) {
	(*this).audio = audio;
}

wstring SystemInfo::getAudio(void) {
	return (*this).audio;
}

wstring SystemInfo::getUptime(void) {
	return (*this).uptime;
}

void SystemInfo::setUptime(wstring uptime) {
	(*this).uptime = uptime;
}

wstring SystemInfo::getBIOS(void) {
	return (*this).BIOS;
}

void SystemInfo::setBIOS(wstring bios) {
	(*this).BIOS = bios;
}

vector<NetAdapter> SystemInfo::getNetworkAdapters(void) {
	return (*this).networkAdapters;
}

void SystemInfo::setSnapshotGenDateTime(wstring snapshotGenDateTime) {
	(*this).snapshotGenDateTime = snapshotGenDateTime;
}

wstring SystemInfo::getSnapshotGenDateTime(void) {
	return (*this).snapshotGenDateTime;
}
