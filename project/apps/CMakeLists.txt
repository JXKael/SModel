# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS
aux_source_directory(. DIR_SRCS)
file(GLOB_RECURSE HDR "*.h")

# 将变量DIR_SRCS中源文件编译成可执行文件
add_executable(SModel ${DIR_SRCS} ${HDR}) # 必须在link_libraries之后

target_link_libraries(SModel Model UI)