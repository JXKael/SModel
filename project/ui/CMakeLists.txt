# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SRCS_UI
aux_source_directory(. DIR_SRCS_UI)
file(GLOB_RECURSE DIR_SRCS_UI "./*.cpp")
file(GLOB_RECURSE HDR_UI "./*.h")

# 将变量DIR_SRCS_HELLOWORLD中源文件编译为共享库
add_library(UI ${DIR_SRCS_UI} ${HDR_UI})

target_link_libraries(UI Qt5::Widgets Qt5::OpenGL)
