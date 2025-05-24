#ifndef MOON_HPP
#define MOON_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

class Moon
{
public:
    Moon(float radius = 0.4f);
    ~Moon(); // Add destructor declaration
    void Draw() const;
    glm::vec3 getDirection() const { return glm::vec3(0.0f, -1.0f, 0.5f); }

private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    void setupMesh(float radius);
};

#endif
