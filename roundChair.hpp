// roundChair.hpp
#ifndef ROUNDCHAIR_HPP
#define ROUNDCHAIR_HPP

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct CylinderData
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> lineVertices;
    std::vector<unsigned int> lineIndices;
};

class RoundChair
{
private:
    // VAOs & VBOs for seat
    GLuint seatVAO, seatVBO, seatEBO;
    GLuint seatLineVAO, seatLineVBO, seatLineEBO;

    // VAOs & VBOs for base
    GLuint baseVAO, baseVBO, baseEBO;
    GLuint baseLineVAO, baseLineVBO, baseLineEBO;

    // geometry data
    CylinderData seatData;
    CylinderData baseData;

    // shader uniform locations
    GLint modelLoc, viewLoc, projLoc, colorLoc;

    CylinderData generateCylinder(float radius, float height);
    void setupBuffers();

public:
    RoundChair();
    ~RoundChair();

    void initialize(GLuint shaderProgram);
    void render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);
    void cleanup();
};

#endif
