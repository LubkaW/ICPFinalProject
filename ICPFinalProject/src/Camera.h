#pragma once

#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;//2.5
const float SENSITIVITY = 0.1f;
const float ZOOM = 100.0f;

class Camera {

public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;


    // constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // Constructor with scalar values
    //Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    glm::mat4 GetViewMatrixWP();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();


};