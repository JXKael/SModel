# 限定CMake最小版本为3.5
cmake_minimum_required(VERSION 3.5)
# 项目名称SModel（Sphere-Meshes Model）
project(SModel LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置camke module路径
set(CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules ${CMAKE_SOURCE_DIR}/cmake/modules)

# >>> --------------------------- 外部项目配置
# --------------------------- OpenGL 4.6
include(cmake/modules/ConfigureOpenGL.cmake)
# --------------------------- Qt 5.13.1
# 修改此路径到你的计算机上的Qt相应文件夹
set(CMAKE_PREFIX_PATH "E:/Env/Qt/5.13.1/5.13.1/msvc2017_64/lib/cmake")
# set(CMAKE_PREFIX_PATH "E:/Env/Qt/5.4.0/5.4/msvc2013_64_opengl/lib/cmake")
include(cmake/modules/ConfigureQt.cmake)
# --------------------------- GLM 0.9.9.6
include(cmake/modules/ConfigureGLM.cmake)
#  --------------------------- eigen 3.3.7
include(cmake/modules/ConfigureEigen.cmake)
# ---------------------------- OpenCV 4.1.1
# you may need to add OpenCV_DIR: ..\OpenCV\OpenCV4.1.1\opencv\build\ to your
# environment variable and add ..\OpenCV\OpenCV4.1.1\opencv\build\x64\vc15\bin\
# to ensure the application to run
include(cmake/modules/ConfigureOpenCV.cmake)

# >>> --------------------------- 子项目配置
add_subdirectory(project/utils)
add_subdirectory(project/model)
add_subdirectory(project/ui)
add_subdirectory(project/apps)
