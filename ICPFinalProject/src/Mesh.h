#pragma once

#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>
#include <string>
#include <vector>
#include "ShaderProgram.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;  // we store the path of the texture to compare with other textures
};


class Mesh {

public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(ShaderProgram& shader);

private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();



};