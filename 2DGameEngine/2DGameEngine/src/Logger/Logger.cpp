#include "../Logger/Logger.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <sstream>

std::vector<LogEntry> Logger::messages;


using namespace std;

string CurrentDateTimeToString()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    struct tm newtime;
    localtime_s(&newtime, &now);
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &newtime);
    return output;
}



    void Logger::Log(const std::string& message) {
        LogEntry logEntry;
        logEntry.type = LOG_INFO;
        logEntry.message = "LOG: [" + CurrentDateTimeToString() + "]: " + message;
        std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;

        messages.push_back(logEntry);
    }

    void Logger::Err(const std::string& message) {
        LogEntry logEntry;
        logEntry.type = LOG_ERROR;
        logEntry.message = "ERR: [" + CurrentDateTimeToString() + "]: " + message;
        std::cerr << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;
    }

      void Logger::Warning(const std::string & message) {
          LogEntry logEntry;
          logEntry.type = LOG_WARNING;
          logEntry.message = "WARN: [" + CurrentDateTimeToString() + "]: " + message;
            std::cerr << "\x1B[93m" << logEntry.message << "\033[0m" << std::endl;
    }

    

