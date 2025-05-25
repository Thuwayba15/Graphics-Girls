#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

// Structure for mesh with texture support
struct TexturedMesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    
    GLuint VAO, VBO, EBO, NBO, TBO;
    GLuint textureID;
    float shininess;
    glm::vec3 specular;
};

void createCafeCounter(std::vector<TexturedMesh>& meshes, float width = 4.0f, float depth = 1.5f, float height = 1.0f);
void createChocolateBarsOnCounter(std::vector<TexturedMesh>& chocolateBars, float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f);
std::vector<glm::mat4> getChocolateBarTransforms(float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f);
void renderTexturedMesh(const TexturedMesh& mesh, GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
void renderChocolateBarsOnCounter(const std::vector<TexturedMesh>& chocolateBars, GLuint shaderProgram, const glm::mat4& baseTransform, const glm::mat4& view, const glm::mat4& projection, float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f);

// Implemented elsewhere
GLuint generateLightGreyMarbleTexture();
GLuint generateDarkShinyGreyTexture();
void calculateNormals(TexturedMesh& mesh);
void setupMeshBuffers(TexturedMesh& mesh);
void generateChocolateBar(TexturedMesh& mesh);
GLuint generateSolidColorTexture(float r, float g, float b);
