# SModel
A Sphere-Meshes based hunman body model with jointed hand

## Running Enviroment
- **Windows 10**
- Intel Core **i7-8700 @ 3.2GhZ**
- Qt 5.13.1
- OpenGL 4.6
- GLM 0.9.9.6
- Eigen 3.3.7

## Getting Started

### Setup

in `CMakeLists.txt`

* `set (CMAKE_PREFIX_PATH ...)` to your Qt path and make sure you have already added the environment value
* modify the `main.cpp`, set the `project_path` to your project
* we have put the third party library in the folder **external**

### Display

* **press 0**: a dialog of tunning the body
* **press 1**: a dialog of tunning the right hand
* **press 2**: a dialog of tunning the left hand

### Running Result

![Result](docs/result.png)

## Model Introduce

![Body](docs/body.png)

The body's structure is similar to the OpenPose, we add 13-20 centers to simulate the arm.

![Hand](docs/hand.png)

This hand model is the same as [HModel](https://github.com/OpenGP/hmodel). Actually our work is based on their work.

