// roof.cpp
#include "roof.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Roof::Roof(float radius, float length, int sectors, int stacks)
    : roofLength(length)
{
    setupRoofMesh(radius, length, sectors, stacks);
    setupCoverMesh(radius, sectors);
}

Roof::~Roof()
{
    // clean up roof mesh
    glDeleteVertexArrays(1, &roofVAO);
    glDeleteBuffers(1, &roofVBO);
    glDeleteBuffers(1, &roofEBO);

    // clean up cover mesh
    glDeleteVertexArrays(1, &coverVAO);
    glDeleteBuffers(1, &coverVBO);
    glDeleteBuffers(1, &coverEBO);
}

void Roof::setupRoofMesh(float radius, float length, int sectors, int stacks)
{
    std::vector<float> vertices;
    for (int i = 0; i <= stacks; ++i)
    {
        float y = (float)i / stacks * length;
        for (int j = 0; j <= sectors; ++j)
        {
            float theta = (float)j / sectors * glm::pi<float>();
            // position
            float x = radius * cos(theta);
            float z = radius * sin(theta);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal (calculated for cylindrical surface)
            vertices.push_back(cos(theta));
            vertices.push_back(0.0f);
            vertices.push_back(sin(theta));
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < sectors; ++j)
        {
            indices.push_back(i * (sectors + 1) + j);
            indices.push_back((i + 1) * (sectors + 1) + j);
            indices.push_back(i * (sectors + 1) + j + 1);
            indices.push_back((i + 1) * (sectors + 1) + j);
            indices.push_back((i + 1) * (sectors + 1) + j + 1);
            indices.push_back(i * (sectors + 1) + j + 1);
        }
    }
    roofIndexCount = indices.size();

    glGenVertexArrays(1, &roofVAO);
    glGenBuffers(1, &roofVBO);
    glGenBuffers(1, &roofEBO);

    glBindVertexArray(roofVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roofVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roofEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Roof::setupCoverMesh(float radius, int sectors)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // center vertex (position + normal)
    vertices.push_back(0.0f); // x
    vertices.push_back(0.0f); // y
    vertices.push_back(0.0f); // z
    vertices.push_back(0.0f); // normal x
    vertices.push_back(1.0f); // normal y
    vertices.push_back(0.0f); // normal z

    // semi-circle edge vertices
    for (int i = 0; i <= sectors; ++i)
    {
        float theta = (float)i / sectors * glm::pi<float>();
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);

        // normal (pointing outward along Y axis for end cap)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
    }

    // create triangles from center to edge vertices
    for (int i = 0; i < sectors; ++i)
    {
        indices.push_back(0);     // center vertex
        indices.push_back(i + 1); // current edge vertex
        indices.push_back(i + 2); // next edge vertex
    }

    coverIndexCount = indices.size();

    glGenVertexArrays(1, &coverVAO);
    glGenBuffers(1, &coverVBO);
    glGenBuffers(1, &coverEBO);

    glBindVertexArray(coverVAO);
    glBindBuffer(GL_ARRAY_BUFFER, coverVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coverEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Roof::DrawRoof() const
{
    glBindVertexArray(roofVAO);
    glDrawElements(GL_TRIANGLES, roofIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Roof::DrawCovers() const
{
    glBindVertexArray(coverVAO);
    glDrawElements(GL_TRIANGLES, coverIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Roof::DrawAll() const
{
    DrawRoof();
    DrawCovers();
}
