# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)

include_directories(../)
# 将当前目录可执行文件添加进变量DIR_SMODEL
aux_source_directory(. DIR_SMODEL)
file(GLOB_RECURSE HDR_SMODEL "*.h")

# 将变量DIR_SMODEL中源文件编译成可执行文件
add_executable(SModel ${DIR_SMODEL} ${HDR_SMODEL}) # 必须在link_libraries之后

target_link_libraries(SModel Utils Model Singer UI ${LIBRARIES})
