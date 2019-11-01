#ifndef CAMERA_H
#define CAMERA_H

#include "qgl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

namespace ui {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
    kUDLR = 0,  // up & down & left & right
    kFB = 1     // forward & back
};

enum CameraType {
    kFocus = 0,
    kFree = 1
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
    // Camera options
    float movement_speed;
    float mouse_sensitivity;
    float fovy;
    float near_plane;
    float far_plane;

    CameraType camera_type;

    // Constructor with vectors
    Camera(glm::vec3 position = POSITION, glm::vec3 focus_position = FOCUS_POSITION, CameraType camera_type = kFree) {
        Reset(position, focus_position, camera_type);
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float at_x, float at_y, float at_z, CameraType camera_type = kFree) {
        Reset(glm::vec3(posX, posY, posZ), glm::vec3(at_x, at_y, at_z), camera_type);
    }

    void Reset(glm::vec3 position = POSITION, glm::vec3 focus_position = FOCUS_POSITION, CameraType camera_type = kFree) {
        this->camera_type = camera_type;
        this->position = position;
        this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = YAW;
        this->pitch = PITCH;
        this->focus_position = focus_position;
        this->near_plane = NEAR_PLANE;
        this->far_plane = FAR_PLANE;
        this->front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->movement_speed = SPEED;
        this->mouse_sensitivity = SENSITIVITY;
        this->fovy = FOVY;
        SetBasePosition();
        SetBaseDirection();
        UpdateCameraVectors();
    }

    // Storage the direction value to move the mouse when you do not release the button
    void SetBaseDirection() {
        yaw_base = yaw;
        pitch_base = pitch;
    }

    void SetBasePosition() {
        position_base = position;
    }

    // Switch camera type
    void SetCameraType(CameraType camera_type) {
        this->camera_type = camera_type;
        this->SetBaseDirection();
    }

    CameraType GetCameraType() {
        return this->camera_type;
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void ProcessPerspective(float xoffset = 0.0f, float yoffset = 0.0f, GLboolean constrain_pitch = true) {
        yaw = yaw_base + xoffset * mouse_sensitivity;
        pitch = pitch_base + yoffset * mouse_sensitivity;

        if (constrain_pitch) pitch = constrainPitch(pitch);

        UpdateCameraVectors();
    }

    void ProcessMovement(CameraMovement move_type, glm::vec2 offset) {
        switch (move_type) {
        case kUDLR:
            {
                glm::vec3 lr = -offset.x * movement_speed * right;
                glm::vec3 ud = offset.y * movement_speed * up;
                glm::vec3 move = lr + ud;
                position = position_base + move;
            }
            break;
        case kFB:
            {
                float ratio = (offset.x + offset.y) / glm::sqrt(glm::pow(offset.x, 2) + glm::pow(offset.y, 2));
                position = position_base + ratio * glm::length(offset) * front;
            }
            break;
        default:
            break;
        }
    }

    void ProcessScroll(float delta) {
        position = position_base + delta * front;
        SetBasePosition();
    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors() {
        switch (camera_type) {
        case kFocus:
            updateFocusCameraVectors();
            break;
        case kFree:
            updateFreeCameraVectors();
            break;
        default:
            break;
        }
    }

private:
    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    float constrainPitch(float pitch) {
        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        return pitch;
    }

    void updateFocusCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 direction(0, 0, 0);
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        direction = glm::normalize(direction);
        front = glm::normalize(focus_position - direction);
        // Also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));

        float distance = glm::length(focus_position - position);
        position = distance * direction;
        SetBasePosition();
    }

    void updateFreeCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 direction(0, 0, 0);
        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        front = glm::normalize(glm::vec3(0, 0, 0) - direction);
        // Also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));

        //float distance = glm::length(glm::vec3(0, 0, 0) - position);
        //position = distance * direction;
        //SetBasePosition();
    }
};

} // namespace ui

#endif
