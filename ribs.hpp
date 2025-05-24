// ribs.hpp
#ifndef RIBS_HPP
#define RIBS_HPP

#include <vector>
#include <GL/glew.h>

class Ribs
{
public:
    Ribs(float radius = 1.0f, float length = 2.0f, int numRibs = 8, int sectors = 30);
    ~Ribs();
    void Draw() const;

private:
    GLuint VAO, VBO;
    size_t vertexCount;

    void setupMesh(float radius, float length, int numRibs, int sectors);
};

#endif
