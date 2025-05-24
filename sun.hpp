#ifndef SUN_HPP
#define SUN_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

class Sun
{
public:
    Sun(float radius = 0.5f);
    void Draw() const;
    glm::vec3 getDirection() const { return glm::vec3(0.0f, -1.0f, -0.5f); } // Example direction

private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    void setupMesh(float radius);
};

#endif
