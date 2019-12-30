#ifndef DIR_UTILS_H
#define DIR_UTILS_H

#include <iostream>
#include <vector>
#include <io.h>

namespace utils {

static std::vector<std::string> listFiles(const char *dir) {
    std::vector<std::string> names;
    char dirNew[200];
    strcpy(dirNew, dir);
    strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dirNew, &findData);
    if (handle == -1)        // 检查是否成功
        return names;

    do {
        if (findData.attrib & _A_SUBDIR) {
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
                continue;

            std::cout << findData.name << "\t<dir>\n";

            // 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
            strcpy(dirNew, dir);
            strcat(dirNew, "\\");
            strcat(dirNew, findData.name);

            listFiles(dirNew);
        } else {
            names.push_back(findData.name);
            // std::cout << findData.name << "\t" << findData.size << " bytes.\n";
        }
    } while (_findnext(handle, &findData) == 0);

    _findclose(handle);    // 关闭搜索句柄
    return names;
}

} // namespace utils

#endif DIR_UTILS_H
