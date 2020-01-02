#ifndef CSV_CONVERT_H
#define CSV_CONVERT_H

#include <vector>
#include <string>
#include <sstream>

namespace utils {

static std::vector<int> ConvertToVectorInt(const std::string &str) {
    std::vector<int> res;
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        while (getline(ss, val, '#')) {
            res.push_back((int)atoi(val.c_str()));
        }
    }
    return res;
}

static std::vector<float> ConvertToVectorFloat(const std::string &str) {
    std::vector<float> res;
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        while (getline(ss, val, '#')) {
            res.push_back((float)atof(val.c_str()));
        }
    }
    return res;
}

static std::vector<std::string> ConvertToVectorString(const std::string &str) {
    std::vector<std::string> res;
    if (str.size() > 0) {
        std::stringstream ss(str);
        std::string val;
        while (getline(ss, val, '#')) {
            res.push_back(val);
        }
    }
    return res;
}

} // namespace utils

#endif // CSV_CONVERT_H
