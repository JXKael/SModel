# SModel
A Sphere-Meshes based hunman body model with jointed hand

## Running Enviroment
To obtain the results shown in the video proper hardware is necessary:
- **Windows 10**
- Intel Core **i7-8700 @ 3.2GhZ**
- Qt 5.13.1
- OpenGL 4.6
- GLM 0.9.9.6
- Eigen 3.3.7

## Getting Started

### Setup

in `CMakeLists.txt`

* `set (CMAKE_PREFIX_PATH ...)` to your Qt path and make sure you have already add the environment value
* we have put the third party library in the folder **external**

### Display

* **press 0**: a dialog of tunning the body
* **press 1**: a dialog of tunning the right hand
* **press 2**: a dialog of tunning the left hand

