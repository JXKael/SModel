#ifndef SMLOG_H
#define SMLOG_H

#include <iostream>
#include <string>

namespace utils {

enum LogType {
    kInfo = 0,
    kWarning,
    kError
};

class SMLog {
private:
    SMLog();
public:
    SMLog(const SMLog &) = delete;
    SMLog &operator=(const SMLog &) = delete;

    static SMLog &logger();

    inline void print(const LogType &log_type, const std::string &content);
    void print(const LogType &log_type);
    inline void print(const std::string &content);
};

} // namespace utils

#endif // SMLOG_H
