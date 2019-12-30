# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS_SINGER
aux_source_directory(. DIR_SRCS_SINGER)
file(GLOB_RECURSE DIR_SRCS_SINGER "./*.cpp")
file(GLOB_RECURSE HDR_SINGER "./*.h")

# 将变量DIR_SRCS_SINGERD中源文件编译为共享库
add_library(Singer ${DIR_SRCS_SINGER} ${HDR_SINGER})
