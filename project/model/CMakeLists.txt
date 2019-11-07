# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS_MODEL
aux_source_directory(. DIR_SRCS_MODEL)
file(GLOB_RECURSE DIR_SRCS_MODEL "./*.cpp")
file(GLOB_RECURSE HDR_MODEL "./*.h")

# 将变量DIR_SRCS_HELLOWORLD中源文件编译为共享库
add_library(Model ${DIR_SRCS_MODEL} ${HDR_MODEL})
