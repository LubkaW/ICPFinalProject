#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

#include "Plane.h"
#include <iostream>

Plane::Plane(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
    Position = position;
    Yaw = yaw;
    Pitch = pitch;
    WorldUp = up;
    updatePlaneVectors();
}

void Plane::ProcessKeyboard(Plane_Movement direction, float deltaTime)
{
    float velocity = 5.0f * deltaTime;
    if (direction == P_FORWARD)
        Pitch += 5 * velocity;
    if (direction == P_BACKWARD)
        Pitch -= 5 * velocity;
    if (direction == P_LEFT)
        Yaw += 10 * velocity;
    if (direction == P_RIGHT)
        Yaw -= 10 * velocity;

    if (Yaw > 360) Yaw -= 360;
    if (Yaw < 0) Yaw += 360;

   
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
    updatePlaneVectors();
}


glm::mat4 Plane::GetViewMatrix()
{
    return glm::lookAt(Position, Position+Front, Up);
}


//void Plane::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
//{
//    xoffset *= MouseSensitivity;
//    yoffset *= MouseSensitivity;
//
//    Yaw += xoffset;
//    Pitch += yoffset;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (constrainPitch)
//    {
//        if (Pitch > 89.0f)
//            Pitch = 89.0f;
//        if (Pitch < -89.0f)
//            Pitch = -89.0f;
//    }
//
//    // update Front, Right and Up Vectors using the updated Euler angles
//    updatePlaneVectors();
//}

void Plane::updatePlaneVectors()
{
    //rychhlost závislá na pitchi
    if (Pitch < 0) {
        MovementSpeed = BaseSpeed + BaseSpeed * -Pitch / 90.0f;
    }
    else if (Pitch > 45) {
        MovementSpeed = BaseSpeed * 0.5;
    }
    else {
        MovementSpeed = BaseSpeed - BaseSpeed * Pitch / 90.0f;
    }
    // calculate the new Front vector
    glm::vec3 front;
    front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}