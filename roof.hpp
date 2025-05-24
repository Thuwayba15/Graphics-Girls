// roof.hpp
#ifndef ROOF_HPP
#define ROOF_HPP

#include <vector>
#include <GL/glew.h>

class Roof
{
public:
    Roof(float radius = 1.0f, float length = 2.0f, int sectors = 30, int stacks = 10);
    ~Roof();
    void DrawRoof() const;
    void DrawCovers() const;
    void DrawAll() const; // draw both roof & covers

private:
    // roof mesh
    GLuint roofVAO, roofVBO, roofEBO;
    size_t roofIndexCount;

    // cover mesh
    GLuint coverVAO, coverVBO, coverEBO;
    size_t coverIndexCount;

    // stored parameters for cover positioning
    float roofLength;

    void setupRoofMesh(float radius, float length, int sectors, int stacks);
    void setupCoverMesh(float radius, int sectors);
};

#endif
