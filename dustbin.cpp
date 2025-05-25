// dustbin.cpp
#include "dustbin.hpp"
#include <iostream>

Dustbin::Dustbin()
{
        glm::vec3 dustbinColor = glm::vec3(0.5f, 0.5f, 0.5f);                      // gray dustbin
        generateCube(1.5f, 2.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), dustbinColor); // generate main dustbin body (rectangular prism)
        generateTopSlots();                                                        // Generate black rectangles on top face for slots
        generateWireframeEdges();                                                  // Generate wireframe edges
        setupMesh();
}

Dustbin::~Dustbin()
{
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &wireframeVAO); // clean wireframe (edge lines) resources
        glDeleteBuffers(1, &wireframeVBO);
}

void Dustbin::generateCube(float width, float height, float depth,
                           glm::vec3 position, glm::vec3 color)
{
        float w = width / 2.0f;
        float h = height / 2.0f;
        float d = depth / 2.0f;

        // def 8 vertices of the cube
        glm::vec3 vertices[8] = {
            position + glm::vec3(-w, -h, -d), // 0
            position + glm::vec3(w, -h, -d),  // 1
            position + glm::vec3(w, h, -d),   // 2
            position + glm::vec3(-w, h, -d),  // 3
            position + glm::vec3(-w, -h, d),  // 4
            position + glm::vec3(w, -h, d),   // 5
            position + glm::vec3(w, h, d),    // 6
            position + glm::vec3(-w, h, d)    // 7
        };

        // front face
        addQuad(vertices[0], vertices[1], vertices[2], vertices[3],
                glm::vec3(0.0f, 0.0f, -1.0f), color);

        // back face
        addQuad(vertices[5], vertices[4], vertices[7], vertices[6],
                glm::vec3(0.0f, 0.0f, 1.0f), color);

        // left face
        addQuad(vertices[4], vertices[0], vertices[3], vertices[7],
                glm::vec3(-1.0f, 0.0f, 0.0f), color);

        // right face
        addQuad(vertices[1], vertices[5], vertices[6], vertices[2],
                glm::vec3(1.0f, 0.0f, 0.0f), color);

        // bottom face
        addQuad(vertices[4], vertices[5], vertices[1], vertices[0],
                glm::vec3(0.0f, -1.0f, 0.0f), color);

        // top face
        addQuad(vertices[3], vertices[2], vertices[6], vertices[7],
                glm::vec3(0.0f, 1.0f, 0.0f), color);
}

void Dustbin::generateTopSlots()
{

        glm::vec3 slotColor = glm::vec3(0.1f, 0.1f, 0.1f);

        float topY = 1.01f;

        float slotWidth = 0.4f;
        float slotDepth = 0.8f;

        float slotPositions[3] = {-0.4f, 0.0f, 0.4f};

        for (int i = 0; i < 3; i++)
        {
                float x = slotPositions[i];
                float w = slotWidth / 2.0f;
                float d = slotDepth / 2.0f;

                // create vertices for slot rectangle
                glm::vec3 v1 = glm::vec3(x - w, topY, -d); // bottom-left
                glm::vec3 v2 = glm::vec3(x + w, topY, -d); // bottom-right
                glm::vec3 v3 = glm::vec3(x + w, topY, d);  // top-right
                glm::vec3 v4 = glm::vec3(x - w, topY, d);  // top-left

                addQuad(v1, v2, v3, v4, glm::vec3(0.0f, 1.0f, 0.0f), slotColor); // add the black rectangle (facing up)
        }
}

void Dustbin::generateWireframeEdges()
{
        // define wireframe edges of dustbin
        float w = 1.5f / 2.0f; // width/2
        float h = 2.0f / 2.0f; // height/2
        float d = 1.0f / 2.0f; // depth/2

        // 8 vertices of cube
        glm::vec3 v[8] = {
            glm::vec3(-w, -h, -d), // 0
            glm::vec3(w, -h, -d),  // 1
            glm::vec3(w, h, -d),   // 2
            glm::vec3(-w, h, -d),  // 3
            glm::vec3(-w, -h, d),  // 4
            glm::vec3(w, -h, d),   // 5
            glm::vec3(w, h, d),    // 6
            glm::vec3(-w, h, d)    // 7
        };

        // def 12 edges of the cube
        unsigned int edges[12][2] = {
            // bottom face edges
            {0, 1},
            {1, 5},
            {5, 4},
            {4, 0},
            // top face edges
            {3, 2},
            {2, 6},
            {6, 7},
            {7, 3},
            // vertical edges
            {0, 3},
            {1, 2},
            {5, 6},
            {4, 7}};

        glm::vec3 wireColor = glm::vec3(0.15f, 0.15f, 0.15f); // wireframe color (dark gray/black)
        glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);

        // create wireframe vertices w same format as main mesh (pos + normal + color)
        for (int i = 0; i < 12; i++)
        {
                // add start vertex (position, normal, color)
                wireframeVertices.insert(wireframeVertices.end(), {v[edges[i][0]].x, v[edges[i][0]].y, v[edges[i][0]].z});
                wireframeVertices.insert(wireframeVertices.end(), {normal.x, normal.y, normal.z});
                wireframeVertices.insert(wireframeVertices.end(), {wireColor.x, wireColor.y, wireColor.z});

                // add end vertex (position, normal, color)
                wireframeVertices.insert(wireframeVertices.end(), {v[edges[i][1]].x, v[edges[i][1]].y, v[edges[i][1]].z});
                wireframeVertices.insert(wireframeVertices.end(), {normal.x, normal.y, normal.z});
                wireframeVertices.insert(wireframeVertices.end(), {wireColor.x, wireColor.y, wireColor.z});
        }
}

void Dustbin::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4,
                      glm::vec3 normal, glm::vec3 color)
{
        unsigned int startIndex = vertices.size() / 9; // 9 floats per vertex (pos + normal + color)

        // add vertices (position, normal, color)
        // v1
        vertices.insert(vertices.end(), {v1.x, v1.y, v1.z});
        vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
        vertices.insert(vertices.end(), {color.x, color.y, color.z});

        // v2
        vertices.insert(vertices.end(), {v2.x, v2.y, v2.z});
        vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
        vertices.insert(vertices.end(), {color.x, color.y, color.z});

        // v3
        vertices.insert(vertices.end(), {v3.x, v3.y, v3.z});
        vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
        vertices.insert(vertices.end(), {color.x, color.y, color.z});

        // v4
        vertices.insert(vertices.end(), {v4.x, v4.y, v4.z});
        vertices.insert(vertices.end(), {normal.x, normal.y, normal.z});
        vertices.insert(vertices.end(), {color.x, color.y, color.z});

        // add indices for 2 triangles
        indices.insert(indices.end(), {startIndex, startIndex + 1, startIndex + 2,
                                       startIndex, startIndex + 2, startIndex + 3});
}

void Dustbin::setupMesh()
{
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // color attribute
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        // wireframe mesh
        glGenVertexArrays(1, &wireframeVAO);
        glGenBuffers(1, &wireframeVBO);

        glBindVertexArray(wireframeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, wireframeVBO);
        glBufferData(GL_ARRAY_BUFFER, wireframeVertices.size() * sizeof(float),
                     wireframeVertices.data(), GL_STATIC_DRAW);

        // position attribute for wireframe (same layout as main mesh)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // normal attribute for wireframe
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // color attribute for wireframe
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
                              (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
}

void Dustbin::render()
{
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
}

void Dustbin::renderWireframe()
{
        // render wireframe edges using GL_LINES
        glBindVertexArray(wireframeVAO);
        glDrawArrays(GL_LINES, 0, wireframeVertices.size() / 9); // 9 floats per vertex
        glBindVertexArray(0);
}
