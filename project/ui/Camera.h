#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <climits>

namespace ui {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
    kUDLR = 0,  // up & down & left & right
    kFB = 1     // forward & back
};

// Default camera values
const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 100.0f);
const glm::vec3 FOCUS_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const float YAW = 90.0f;
const float PITCH = 0.95493f;
const float SENSITIVITY = 0.1f; // 旋转镜头
const float SPEED = 0.7f; // 平移镜头
const float FOVY = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 10000.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 position_base;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // Euler Angles
    float yaw;
    float pitch;
    float yaw_base;
    float pitch_base;
    glm::vec3 focus_position;
    float distance;
    // Camera options
    float movement_speed;
    float mouse_sensitivity;
    float fovy;
    float near_plane;
    float far_plane;

public:
    // Constructor with vectors
    Camera(glm::vec3 position = POSITION, glm::vec3 focus_position = FOCUS_POSITION);
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float at_x, float at_y, float at_z);
    void Reset(glm::vec3 position = POSITION, glm::vec3 focus_position = FOCUS_POSITION);

    // Storage the Camera Position as moving the mouse while you do not release the mouse button
    void SetBasePosition();
    // Storage the Euler Angles as moving the mouse while you do not release the mouse button
    void SetBaseEulerAngles();
    void SetEulerAngles(const float yaw, const float pitch);
    // Calculate the front vector from the Camera's (updated) Euler Angles
    void UpdateEyeCameraVectors();
    void UpdateFocusCameraVectors();

    // Process the event from window
    void ProcessEyePerspective(float xoffset = 0.0f, float yoffset = 0.0f, bool constrain_pitch = true);
    void ProcessFocusPerspective(float xoffset = 0.0f, float yoffset = 0.0f, bool constrain_pitch = true);
    void ProcessMovement(CameraMovement move_type, glm::vec2 offset);
    void ProcessScroll(float delta);

    // Return the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

private:
    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    float constrainPitch(float pitch);
    void updateDistance();
    void updateFocusPosition();
};

} // namespace ui

#endif // CAMERA_H
