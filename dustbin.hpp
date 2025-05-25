// dustbin.hpp
#ifndef DUSTBIN_HPP
#define DUSTBIN_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Dustbin
{
public:
    Dustbin();
    ~Dustbin();
    void render();
    void renderWireframe();

private:
    // Main mesh resources
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Wireframe mesh resources
    unsigned int wireframeVAO, wireframeVBO;
    std::vector<float> wireframeVertices;

    void setupMesh();
    void generateCube(float width, float height, float depth,
                      glm::vec3 position, glm::vec3 color);
    void generateTopSlots();
    void generateWireframeEdges();
    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4,
                 glm::vec3 normal, glm::vec3 color);
};

#endif
