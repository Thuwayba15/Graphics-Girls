// highChairs.cpp
#include "highChairs.hpp"
#include <cmath>

HighChair::HighChair() : VAO(0), VBO(0), EBO(0), shaderProgram(0)
{
}

HighChair::~HighChair()
{
    cleanup();
}

void HighChair::generateChairGeometry()
{
    vertices.clear();
    indices.clear();

    const float seatWidth = 1.0f;
    const float seatDepth = 1.0f;
    const float seatHeight = 0.2f;
    const float seatY = 2.0f;

    const float backrestWidth = 1.0f;
    const float backrestHeight = 1.5f;
    const float backrestThickness = 0.1f;

    const float legRadius = 0.08f;
    const float legHeight = 2.0f;

    // helper funct. to add a white box
    auto addBox = [&](float x, float y, float z, float w, float h, float d)
    {
        unsigned int startIdx = vertices.size() / 9; // 9 elements per vertex now

        // 8 vertices with normals & white color
        float boxVerts[] = {
            // Front face (white)
            x - w / 2, y - h / 2, z + d / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            x + w / 2, y - h / 2, z + d / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            x + w / 2, y + h / 2, z + d / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            x - w / 2, y + h / 2, z + d / 2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            // Back face (white)
            x - w / 2, y - h / 2, z - d / 2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            x + w / 2, y - h / 2, z - d / 2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            x + w / 2, y + h / 2, z - d / 2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f,
            x - w / 2, y + h / 2, z - d / 2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f};

        for (int i = 0; i < 72; i++)
        { // 8 vertices * 9 components
            vertices.push_back(boxVerts[i]);
        }

        // box faces indices (same as before)
        unsigned int boxIndices[] = {
            0, 1, 2, 2, 3, 0, 4, 7, 6, 6, 5, 4,
            4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1,
            3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4};

        for (int i = 0; i < 36; i++)
        {
            indices.push_back(startIdx + boxIndices[i]);
        }
    };

    // helper to add dark gray cylinder
    auto addCylinder = [&](float x, float y, float z, float radius, float height)
    {
        unsigned int startIdx = vertices.size() / 9;
        const int segments = 8;
        glm::vec3 legColor(0.2f, 0.2f, 0.2f); // dark gray

        // top & bottom centers
        vertices.insert(vertices.end(), {x, y + height / 2, z, 0.0f, 1.0f, 0.0f, legColor.r, legColor.g, legColor.b});
        vertices.insert(vertices.end(), {x, y - height / 2, z, 0.0f, -1.0f, 0.0f, legColor.r, legColor.g, legColor.b});

        for (int i = 0; i < segments; i++)
        {
            float angle = 2.0f * M_PI * i / segments;
            float cx = x + radius * cos(angle);
            float cz = z + radius * sin(angle);
            float nx = cos(angle);
            float nz = sin(angle);

            // top rim (dark gray)
            vertices.insert(vertices.end(), {cx, y + height / 2, cz, nx, 0.0f, nz, legColor.r, legColor.g, legColor.b});
            // bottom rim (dark gray)
            vertices.insert(vertices.end(), {cx, y - height / 2, cz, nx, 0.0f, nz, legColor.r, legColor.g, legColor.b});
        }

        // create faces
        for (int i = 0; i < segments; i++)
        {
            int next = (i + 1) % segments;
            int topRim = 2 + i * 2;
            int bottomRim = 2 + i * 2 + 1;
            int nextTopRim = 2 + next * 2;
            int nextBottomRim = 2 + next * 2 + 1;

            indices.insert(indices.end(), {startIdx, startIdx + topRim, startIdx + nextTopRim});
            indices.insert(indices.end(), {startIdx + 1, startIdx + nextBottomRim, startIdx + bottomRim});
            indices.insert(indices.end(), {startIdx + topRim, startIdx + bottomRim, startIdx + nextBottomRim});
            indices.insert(indices.end(), {startIdx + topRim, startIdx + nextBottomRim, startIdx + nextTopRim});
        }
    };

    // Build chair components:
    // 1. white seat
    addBox(0.0f, seatY, 0.0f, seatWidth, seatHeight, seatDepth);

    // 2. white backrest
    addBox(0.0f, seatY + seatHeight / 2 + backrestHeight / 2, -seatDepth / 2 + backrestThickness / 2,
           backrestWidth, backrestHeight, backrestThickness);

    // 3. dark gray legs
    float legOffset = seatWidth / 2 - legRadius;
    addCylinder(-legOffset, legHeight / 2, -legOffset, legRadius, legHeight);
    addCylinder(legOffset, legHeight / 2, -legOffset, legRadius, legHeight);
    addCylinder(-legOffset, legHeight / 2, legOffset, legRadius, legHeight);
    addCylinder(legOffset, legHeight / 2, legOffset, legRadius, legHeight);

    // 4. white footrest
    addBox(0.0f, 0.8f, legOffset, seatWidth * 0.8f, 0.05f, 0.05f);

    // 5. white safety bar
    addBox(0.0f, seatY + seatHeight / 2 + 0.3f, seatDepth / 2 + 0.05f, seatWidth * 0.8f, 0.05f, 0.05f);

    // 6. white armrests
    addBox(-seatWidth / 2 + 0.05f, seatY + seatHeight / 2 + 0.2f, 0.0f, 0.1f, 0.4f, seatDepth * 0.8f);
    addBox(seatWidth / 2 - 0.05f, seatY + seatHeight / 2 + 0.2f, 0.0f, 0.1f, 0.4f, seatDepth * 0.8f);
}

void HighChair::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // normal (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // colour (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void HighChair::initialize(GLuint shader)
{
    shaderProgram = shader;
    generateChairGeometry();
    setupBuffers();
}

void HighChair::render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
{
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HighChair::cleanup()
{
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}
