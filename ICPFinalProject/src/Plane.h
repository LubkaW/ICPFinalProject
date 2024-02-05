#pragma once

#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

enum Plane_Movement {
    P_FORWARD,
    P_BACKWARD,
    P_LEFT,
    P_RIGHT
};



class Plane {

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
    float BaseSpeed = 8.0f;
    float MovementSpeed = 8.0f;



    // constructor with vectors
    Plane(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // Constructor with scalar values

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Plane_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updatePlaneVectors();


};