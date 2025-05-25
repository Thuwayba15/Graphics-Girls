// ribs.cpp
#include "ribs.hpp"
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Ribs::Ribs(float radius, float length, int numRibs, int sectors)
{
    setupMesh(radius, length, numRibs, sectors);
}

Ribs::~Ribs()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Ribs::setupMesh(float radius, float length, int numRibs, int sectors)
{
    std::vector<float> vertices;

    // cross-sectional ribs (perpendicular to length)
    for (int rib = 0; rib <= numRibs; ++rib)
    {
        float y = (float)rib / numRibs * length;

        // curved rib structure
        for (int j = 0; j <= sectors; ++j)
        {
            float theta = (float)j / sectors * glm::pi<float>();
            float x = radius * cos(theta);
            float z = radius * sin(theta);

            // add position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // longitudinal ribs (parallel to length) 
    int longitudinalRibs = 5; // bottom, 2 sides, & 2 intermediate
    for (int longRib = 0; longRib < longitudinalRibs; ++longRib)
    {
        float theta;
        switch (longRib)
        {
        case 0:
            theta = 0.0f;
            break; // bottom center
        case 1:
            theta = glm::pi<float>() * 0.25f;
            break; // quarter way up
        case 2:
            theta = glm::pi<float>() * 0.75f;
            break; // 3-quarters way up
        case 3:
            theta = glm::pi<float>() * 0.5f;
            break; // top center
        case 4:
            theta = glm::pi<float>();
            break; // very top
        }

        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // longitudinal rib along the length
        for (int i = 0; i <= 20; ++i) // more segments for smooth longitudinal ribs
        {
            float y = (float)i / 20.0f * length;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    vertexCount = vertices.size() / 3;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Ribs::Draw() const
{
    glBindVertexArray(VAO);

    // draw cross-sectional ribs as line strips
    int sectors = 30;
    int numRibs = 8;

    for (int rib = 0; rib <= numRibs; ++rib)
    {
        glDrawArrays(GL_LINE_STRIP, rib * (sectors + 1), sectors + 1);
    }

    // draw longitudinal ribs as line strips
    int startIdx = (numRibs + 1) * (sectors + 1);
    int longitudinalRibs = 5;

    for (int longRib = 0; longRib < longitudinalRibs; ++longRib)
    {
        glDrawArrays(GL_LINE_STRIP, startIdx + longRib * 21, 21);
    }

    glBindVertexArray(0);
}
