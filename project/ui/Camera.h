#ifndef CAMERA_H
#define CAMERA_H

#include "qgl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace ui {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.3f;
const float FOVY = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 10000.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // Euler Angles
    float yaw;
    float pitch;
    float initial_yaw;
    float initial_pitch;
    glm::vec3 lookat_position;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float fovy;
    float near_plane;
    float far_plane;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 lookat_position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
      : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        fovy(FOVY)
    {
        this->position = position;
        this->lookat_position = lookat_position;
        this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = yaw;
        this->pitch = pitch;
        this->near_plane = NEAR_PLANE;
        this->far_plane = FAR_PLANE;
        SetInitialDirection();
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float at_x, float at_y, float at_z, float yaw, float pitch)
      : front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        fovy(FOVY)
    {
        position = glm::vec3(posX, posY, posZ);
        lookat_position = glm::vec3(at_x, at_y, at_z);
        world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = yaw;
        this->pitch = pitch;
        this->near_plane = NEAR_PLANE;
        this->far_plane = FAR_PLANE;
        SetInitialDirection();
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        yaw = initial_yaw + xoffset * MouseSensitivity;
        pitch = initial_pitch + yoffset * MouseSensitivity;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }
    
    // Storage the direction value to move the mouse when you do not release the button
    void SetInitialDirection() {
        initial_yaw = yaw;
        initial_pitch = pitch;
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
        if (fovy >= 1.0f && fovy <= 45.0f)
            fovy -= yoffset;
        if (fovy <= 1.0f)
            fovy = 1.0f;
        if (fovy >= 45.0f)
            fovy = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // Calculate the new Front vector
        float distance = glm::length(lookat_position - position);
        glm::vec3 camera_pos(0, 0, 0);
        float a = cos(glm::radians(yaw));
        float b = cos(glm::radians(pitch));
        camera_pos.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        camera_pos.y = sin(glm::radians(pitch));
        camera_pos.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        camera_pos = glm::normalize(camera_pos);
        front = glm::normalize(lookat_position - camera_pos);
        // Also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));

        position = distance * camera_pos;
    }
};

} // namespace ui

#endif
