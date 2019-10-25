# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS_HELLOWORLD
aux_source_directory(. DIR_SRCS_Model)
file(GLOB_RECURSE DIR_SRCS_Model "./*.cpp")
file(GLOB_RECURSE HDR_MODEL "./*.h")

# 将变量DIR_SRCS_HELLOWORLD中源文件编译为共享库
add_library(Model ${DIR_SRCS_Model} ${HDR_MODEL})
