#include "SMLog.h"
using namespace utils;

SMLog::SMLog() {}

SMLog &SMLog::logger() {
    static SMLog instance;
    return instance;
}

void SMLog::print(const LogType &log_type, const std::string &content) {
    print(log_type);
    print(content);
    std::cout << std::endl;
}

void SMLog::print(const LogType &log_type) {
    switch (log_type) {
    case LogType::kInfo:
        std::cout << "--> [info]: ";
        break;
    case LogType::kWarning:
        std::cout << "!!! [warning]: ";
        break;
    case LogType::kError:
        std::cout << "@#@ [error]: ";
        break;
    default: break;
    }
}

void SMLog::print(const std::string &content) {
    std::cout << content;
}
