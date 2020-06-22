#include "logger.h"

SimpleLogger* SimpleLogger::loggerInstance;
SimpleLogger* SimpleLogger::getLoggerInstance() {
	if (loggerInstance == nullptr) {
		loggerInstance = new SimpleLogger();
	}
	return loggerInstance;
}

void SimpleLogger::info() {

}

void SimpleLogger::error() {

}

void SimpleLogger::debug() {

}