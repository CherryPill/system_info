#pragma once
#include <fstream>
#include <codecvt>
#include <tchar.h>
class SimpleLogger {
private:
	static SimpleLogger*loggerInstance;
	std::wofstream outLoggerStream;
	TCHAR *hardCodedLoggerOutFilePath = L"";
	SimpleLogger() {
		std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>); 
		outLoggerStream.open(hardCodedLoggerOutFilePath, std::wofstream::out);
		outLoggerStream.imbue(loc); //todo create configAppData directory on application startup
	}
public:
	static SimpleLogger *getLoggerInstance();
	static void info();
	static void error();
	static void debug();

};