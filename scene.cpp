#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "scene.hpp"
#include "shader.hpp"
#include "box.hpp"
#include "texture_loader.hpp"

static GLuint floorVAO, floorVBO, floorEBO;
static GLuint carpetTexture;

void setupScene() {
    float floorVertices[] = {
        -4.0f, 0.0f,  1.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         4.0f, 0.0f,  1.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         4.0f, 0.0f, -31.5f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        -4.0f, 0.0f, -31.5f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glGenBuffers(1, &floorEBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    carpetTexture = loadTexture("carpet.jpeg");
}

void drawScene(GLuint shader) {
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    glm::mat4 floorModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &floorModel[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"), 1.0f, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glUniform1i(glGetUniformLocation(shader, "texture1"), 0);
    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);

    drawBox(shader, glm::vec3(0.0f, 0.01f, 0.0f), glm::vec3(8.0f, 0.01f, 0.1f), glm::vec3(1.0f, 1.0f, 0.0f));

    drawBox(shader, glm::vec3(0.0f, 4.5f, -31.01f), glm::vec3(8.0f, 9.0f, 0.01f), glm::vec3(0.4f));
    drawBox(shader, glm::vec3(0.0f, 4.5f, -31.0f), glm::vec3(5.0f, 9.0f, 0.1f), glm::vec3(0.4f));
    drawBox(shader, glm::vec3(-3.125f, 4.5f, -31.0f), glm::vec3(0.75f, 9.0f, 0.1f), glm::vec3(0.2f));
    drawBox(shader, glm::vec3(3.125f, 4.5f, -31.0f), glm::vec3(0.75f, 9.0f, 0.1f), glm::vec3(0.2f));

    for (int i = 2; i < 4; ++i) {
        float y = 1.3f + i * 2.2f;
        drawBox(shader, glm::vec3(-1.5f, y, -30.95f), glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
        drawBox(shader, glm::vec3(1.5f, y, -30.95f), glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
    }
    for (int i = 0; i < 2; ++i) {
        float y = 1.0f + i * 2.2f;
        drawBox(shader, glm::vec3(-1.5f, y, -30.95f), glm::vec3(1.0f, 2.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
        drawBox(shader, glm::vec3(1.5f, y, -30.95f), glm::vec3(1.0f, 2.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
    }

    auto drawWallWithZigzagAndContours = [&](float xPos, float dir) {
        // Base grey backing
        drawBox(shader, glm::vec3(xPos, 4.5f, -15.0f), glm::vec3(0.01f, 9.0f, 33.0f), glm::vec3(0.85f));

        // Protruding mid off-white strip
        drawBox(shader, glm::vec3(xPos + 0.15f * dir, 4.5f, -15.0f), glm::vec3(0.25f, 2.0f, 33.0f), glm::vec3(0.9f));

        // Top darker grey protrusion
        drawBox(shader, glm::vec3(xPos + 0.1f * dir, 7.0f, -15.0f), glm::vec3(0.2f, 2.0f, 33.0f), glm::vec3(0.55f));

        // Windows between the two protrusions
        drawBox(shader, glm::vec3(xPos + 0.02f * dir, 5.9f, -15.0f), glm::vec3(0.01f, 1.2f, 33.0f), glm::vec3(0.6f, 0.8f, 1.0f));

        // Re-add the wooden slats
        const int numSlats = 40;
        float baseZ = -31.5f;
        float spacing = 33.0f / numSlats;

        for (int i = 0; i < numSlats; ++i) {
            float z = baseZ + i * spacing + spacing / 2.0f;
            float wave = sin(i * 0.2f) + cos(i * 0.1f);
            float height = 1.2f + fabs(wave * 1.1f);

            drawBox(
                shader,
                glm::vec3(xPos + 0.01f * dir, height / 2.0f, z),
                glm::vec3(0.07f, height, spacing * 1.05f),
                glm::vec3(0.4f, 0.2f, 0.1f)
            );
        }
    };

    // Left = North (x = -4), Right = South (x = 4)
    drawWallWithZigzagAndContours(-4.0f, 1.0f);
    drawWallWithZigzagAndContours(4.0f, -1.0f);
}
