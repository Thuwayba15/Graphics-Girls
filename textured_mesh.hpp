#ifndef TEXTURED_MESH_HPP
#define TEXTURED_MESH_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

// A mesh with position, normals, texture coordinates, and OpenGL buffer IDs
struct TexturedMesh {
    std::vector<float> vertices;   // 3 floats per vertex
    std::vector<float> normals;    // 3 floats per normal
    std::vector<float> texCoords;  // 2 floats per texture coord
    std::vector<unsigned int> indices;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint NBO = 0;
    GLuint TBO = 0;
    GLuint EBO = 0;

    GLuint textureID = 0;

    float shininess = 32.0f;
    glm::vec3 specular = glm::vec3(0.5f);
};

void setupMeshBuffers(TexturedMesh& mesh);

#endif
