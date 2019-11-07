# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS_UTILS
aux_source_directory(. DIR_SRCS_UTILS)
file(GLOB_RECURSE DIR_SRCS_UTILS "./*.cpp")
file(GLOB_RECURSE HDR_UTILS "./*.h")

# 将变量DIR_SRCS_UTILS中源文件编译为共享库
add_library(Utils ${DIR_SRCS_UTILS} ${HDR_UTILS})
