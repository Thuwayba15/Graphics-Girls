// roundChair.cpp
#include "roundChair.hpp"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int CYLINDER_SEGMENTS = 32;

RoundChair::RoundChair()
{
    // init. all handles to 0
    seatVAO = seatVBO = seatEBO = 0;
    seatLineVAO = seatLineVBO = seatLineEBO = 0;
    baseVAO = baseVBO = baseEBO = 0;
    baseLineVAO = baseLineVBO = baseLineEBO = 0;
    modelLoc = viewLoc = projLoc = colorLoc = -1;
}

RoundChair::~RoundChair()
{
    cleanup();
}

CylinderData RoundChair::generateCylinder(float radius, float height)
{
    CylinderData data;

    // Generate vertices for cylinder
    // bottom center
    data.vertices.insert(data.vertices.end(), {0.0f, 0.0f, 0.0f});
    // top center
    data.vertices.insert(data.vertices.end(), {0.0f, height, 0.0f});

    // bottom circle vertices
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        float angle = 2.0f * M_PI * i / CYLINDER_SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        data.vertices.insert(data.vertices.end(), {x, 0.0f, z});
    }

    // top circle vertices
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        float angle = 2.0f * M_PI * i / CYLINDER_SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        data.vertices.insert(data.vertices.end(), {x, height, z});
    }

    // Generate indices for triangles
    // bottom face
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        int next = (i + 1) % CYLINDER_SEGMENTS;
        data.indices.insert(data.indices.end(), {0, (unsigned int)(2 + i), (unsigned int)(2 + next)});
    }

    // top face
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        int next = (i + 1) % CYLINDER_SEGMENTS;
        data.indices.insert(data.indices.end(), {1, (unsigned int)(2 + CYLINDER_SEGMENTS + next), (unsigned int)(2 + CYLINDER_SEGMENTS + i)});
    }

    // side faces
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        int next = (i + 1) % CYLINDER_SEGMENTS;
        int bottom1 = 2 + i;
        int bottom2 = 2 + next;
        int top1 = 2 + CYLINDER_SEGMENTS + i;
        int top2 = 2 + CYLINDER_SEGMENTS + next;

        // 2 triangles per side face
        data.indices.insert(data.indices.end(), {(unsigned int)bottom1, (unsigned int)top1, (unsigned int)bottom2});
        data.indices.insert(data.indices.end(), {(unsigned int)bottom2, (unsigned int)top1, (unsigned int)top2});
    }

    // Generate line vertices and indices for edges
    // copy all vertices for lines
    data.lineVertices = data.vertices;

    // bottom circle edges
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        int next = (i + 1) % CYLINDER_SEGMENTS;
        data.lineIndices.insert(data.lineIndices.end(), {(unsigned int)(2 + i), (unsigned int)(2 + next)});
    }

    // top circle edges
    for (int i = 0; i < CYLINDER_SEGMENTS; i++)
    {
        int next = (i + 1) % CYLINDER_SEGMENTS;
        data.lineIndices.insert(data.lineIndices.end(), {(unsigned int)(2 + CYLINDER_SEGMENTS + i), (unsigned int)(2 + CYLINDER_SEGMENTS + next)});
    }

    // vertical edges
    for (int i = 0; i < CYLINDER_SEGMENTS; i += 4)
    { // only every 4th vertical line
        data.lineIndices.insert(data.lineIndices.end(), {(unsigned int)(2 + i), (unsigned int)(2 + CYLINDER_SEGMENTS + i)});
    }

    return data;
}

void RoundChair::setupBuffers()
{
    // create VAOs & VBOs for seat
    glGenVertexArrays(1, &seatVAO);
    glGenBuffers(1, &seatVBO);
    glGenBuffers(1, &seatEBO);

    glBindVertexArray(seatVAO);
    glBindBuffer(GL_ARRAY_BUFFER, seatVBO);
    glBufferData(GL_ARRAY_BUFFER, seatData.vertices.size() * sizeof(float), seatData.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seatEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, seatData.indices.size() * sizeof(unsigned int), seatData.indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // create VAOs & VBOs for base
    glGenVertexArrays(1, &baseVAO);
    glGenBuffers(1, &baseVBO);
    glGenBuffers(1, &baseEBO);

    glBindVertexArray(baseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, baseVBO);
    glBufferData(GL_ARRAY_BUFFER, baseData.vertices.size() * sizeof(float), baseData.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baseEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, baseData.indices.size() * sizeof(unsigned int), baseData.indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // create VAOs & VBOs for seat lines
    glGenVertexArrays(1, &seatLineVAO);
    glGenBuffers(1, &seatLineVBO);
    glGenBuffers(1, &seatLineEBO);

    glBindVertexArray(seatLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, seatLineVBO);
    glBufferData(GL_ARRAY_BUFFER, seatData.lineVertices.size() * sizeof(float), seatData.lineVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seatLineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, seatData.lineIndices.size() * sizeof(unsigned int), seatData.lineIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // create VAOs & VBOs for base lines
    glGenVertexArrays(1, &baseLineVAO);
    glGenBuffers(1, &baseLineVBO);
    glGenBuffers(1, &baseLineEBO);

    glBindVertexArray(baseLineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, baseLineVBO);
    glBufferData(GL_ARRAY_BUFFER, baseData.lineVertices.size() * sizeof(float), baseData.lineVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baseLineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, baseData.lineIndices.size() * sizeof(unsigned int), baseData.lineIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void RoundChair::initialize(GLuint shaderProgram)
{
    // Generate cylinder geometry
    seatData = generateCylinder(1.5f, 0.3f); // Seat: radius 1.5, height 0.3
    baseData = generateCylinder(1.2f, 0.6f); // Base: radius 1.2, height 0.6

    setupBuffers();

    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projLoc = glGetUniformLocation(shaderProgram, "projection");
    colorLoc = glGetUniformLocation(shaderProgram, "color");
}

void RoundChair::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Render seat
    glm::mat4 seatModel = model * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.6f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(seatModel));
    glUniform3f(colorLoc, 0.9f, 0.7f, 0.2f); // Yellow color
    glBindVertexArray(seatVAO);
    glDrawElements(GL_TRIANGLES, seatData.indices.size(), GL_UNSIGNED_INT, 0);

    // Render base
    glm::mat4 baseModel = model;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseModel));
    glUniform3f(colorLoc, 0.8f, 0.6f, 0.1f); // Darker yellow
    glBindVertexArray(baseVAO);
    glDrawElements(GL_TRIANGLES, baseData.indices.size(), GL_UNSIGNED_INT, 0);
    // seat lines
    glUniform3f(colorLoc, 0.4f, 0.3f, 0.1f); // Dark brown for lines
    glLineWidth(2.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(seatModel));
    glBindVertexArray(seatLineVAO);
    glDrawElements(GL_LINES, seatData.lineIndices.size(), GL_UNSIGNED_INT, 0);

    // base lines
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseModel));
    glBindVertexArray(baseLineVAO);
    glDrawElements(GL_LINES, baseData.lineIndices.size(), GL_UNSIGNED_INT, 0);
}

void RoundChair::cleanup()
{
    if (seatVAO != 0)
    {
        glDeleteVertexArrays(1, &seatVAO);
        glDeleteBuffers(1, &seatVBO);
        glDeleteBuffers(1, &seatEBO);
        seatVAO = seatVBO = seatEBO = 0;
    }

    if (baseVAO != 0)
    {
        glDeleteVertexArrays(1, &baseVAO);
        glDeleteBuffers(1, &baseVBO);
        glDeleteBuffers(1, &baseEBO);
        baseVAO = baseVBO = baseEBO = 0;
    }

    if (seatLineVAO != 0)
    {
        glDeleteVertexArrays(1, &seatLineVAO);
        glDeleteBuffers(1, &seatLineVBO);
        glDeleteBuffers(1, &seatLineEBO);
        seatLineVAO = seatLineVBO = seatLineEBO = 0;
    }

    if (baseLineVAO != 0)
    {
        glDeleteVertexArrays(1, &baseLineVAO);
        glDeleteBuffers(1, &baseLineVBO);
        glDeleteBuffers(1, &baseLineEBO);
        baseLineVAO = baseLineVBO = baseLineEBO = 0;
    }
}
