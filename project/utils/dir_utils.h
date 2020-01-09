#ifndef DIR_UTILS_H
#define DIR_UTILS_H

#include <iostream>
#include <vector>
#include <io.h>

namespace utils {

using FileName = std::string;
using FilesList = std::vector<FileName>;

static FilesList list_files(const char *dir) {
    FilesList names;
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

            list_files(dirNew);
        } else {
            names.push_back(findData.name);
            // std::cout << findData.name << "\t" << findData.size << " bytes.\n";
        }
    } while (_findnext(handle, &findData) == 0);

    _findclose(handle);    // 关闭搜索句柄
    return names;
}

static std::string get_data_dir(const std::string &project_dir) {
    return project_dir + "data/";
}

static std::string get_models_dir(const std::string &project_dir) {
    return get_data_dir(project_dir) + "models/";
}

} // namespace utils

#endif DIR_UTILS_H
