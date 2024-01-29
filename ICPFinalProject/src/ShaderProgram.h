#pragma once

#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <GLFW/glfw3.h> // library for app window creation
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram {

public:
    // the program ID
    unsigned int ID;

    // input: file path of vertex and fragment shader
	ShaderProgram(const char* vertexPath, const char* fragmentPath);
	~ShaderProgram();
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 &mat) const;
    void setVec3(const std::string& name, glm::vec3& vec) const;
    void setVec3(const std::string& name, float x, float y, float z) const;


};
