#pragma once

#include <string>
#include <vector>

enum LogType {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry {
	LogType type; // this creates a variable where the error will be one of 3 log types
	std::string message;
};

using namespace std;

class Logger {
public:
	static std::vector<LogEntry> messages;
	static void Log(const string& message);
	static void Err(const std::string& message);
	static void Warning(const std::string& message);
};