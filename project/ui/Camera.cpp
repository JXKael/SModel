#include "Camera.h"

using namespace ui;

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 focus_position) {
    Reset(position, focus_position);
}

// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float at_x, float at_y, float at_z) {
    Reset(glm::vec3(posX, posY, posZ), glm::vec3(at_x, at_y, at_z));
}

void Camera::Reset(glm::vec3 position, glm::vec3 focus_position) {
    this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    this->position = position;
    this->focus_position = focus_position;

    updateDistance();
    this->front = glm::normalize(focus_position - position);
    this->right = glm::normalize(glm::cross(front, world_up));
    this->up = glm::normalize(glm::cross(right, front));
    this->pitch = constrainPitch(glm::degrees(glm::asin(front.y)));
    float yaw_sign = glm::abs(front.z - 0.0f) <= 1e-6 ? 1.0f : glm::sign(front.z);
    this->yaw = yaw_sign * glm::degrees(glm::acos(front.x / cos(glm::radians(pitch))));

    this->near_plane = NEAR_PLANE;
    this->far_plane = FAR_PLANE;
    this->movement_speed = SPEED;
    this->mouse_sensitivity = SENSITIVITY;
    this->fovy = FOVY;

    SetBasePosition();
    SetBaseEulerAngles();
}

// Storage the camera position as moving the mouse while you do not release the mouse button
void Camera::SetBasePosition() {
    position_base = position;
}

// Storage the direction value as moving the mouse while you do not release the mouse button
void Camera::SetBaseEulerAngles() {
    yaw_base = yaw;
    pitch_base = pitch;
}

void Camera::SetEulerAngles(const float yaw, const float pitch) {
    this->yaw = yaw;
    this->pitch = constrainPitch(pitch);
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateEyeCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 direction(0, 0, 0);
    direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    direction = glm::normalize(direction);

    front = direction;
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));

    updateFocusPosition();
    // std::cout << "(" << focus_position.x << ", " << focus_position.y << ", " << focus_position.z << ")" << std::endl;
}

void Camera::UpdateFocusCameraVectors() {
    // Calculate the new Front vector
    float pitch_prime = -pitch;
    float yaw_sign = -(glm::abs(yaw - 0.0f) <= 1e-6 ? 1.0f : glm::sign(yaw));
    float yaw_prime = yaw_sign * (180 - glm::abs(yaw));

    glm::vec3 direction(0, 0, 0);
    direction.x = cos(glm::radians(pitch_prime)) * cos(glm::radians(yaw_prime));
    direction.y = sin(glm::radians(pitch_prime));
    direction.z = cos(glm::radians(pitch_prime)) * sin(glm::radians(yaw_prime));
    direction = -glm::normalize(direction);

    front = direction;
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));

    position = focus_position - front * distance;
}


void Camera::ProcessEyePerspective(float xoffset, float yoffset, bool constrain_pitch) {
    yaw = yaw_base + xoffset * mouse_sensitivity;
    pitch = pitch_base - yoffset * mouse_sensitivity;

    if (constrain_pitch) pitch = constrainPitch(pitch);

    UpdateEyeCameraVectors();
}

void Camera::ProcessFocusPerspective(float xoffset, float yoffset, bool constrain_pitch) {
    yaw = yaw_base + xoffset * mouse_sensitivity;
    pitch = pitch_base - yoffset * mouse_sensitivity;

    if (constrain_pitch) pitch = constrainPitch(pitch);

    UpdateFocusCameraVectors();
}

void Camera::ProcessMovement(CameraMovement move_type, glm::vec2 offset) {
    switch (move_type) {
    case kUDLR:  // 上下左右
        {
            glm::vec3 lr = -offset.x * movement_speed * right;
            glm::vec3 ud = offset.y * movement_speed * up;
            glm::vec3 move = lr + ud;
            position = position_base + move;
            updateFocusPosition();
        }
        break;
    case kFB:  // 前后
        {
            float ratio = (offset.x + offset.y) / glm::sqrt(glm::pow(offset.x, 2) + glm::pow(offset.y, 2));
            position = position_base + ratio * glm::length(offset) * front;
            updateDistance();
        }
        break;
    default:
        break;
    }
}

void Camera::ProcessScroll(float delta) {
    position = position_base + glm::sign(delta) * movement_speed * 20.0f * front;
    updateDistance();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

// Make sure that when pitch is out of bounds, screen doesn't get flipped
float Camera::constrainPitch(float pitch) {
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    return pitch;
}

void Camera::updateDistance() {
    distance = glm::length(focus_position - position);
}

void Camera::updateFocusPosition() {
    focus_position = position + front * distance;
}
