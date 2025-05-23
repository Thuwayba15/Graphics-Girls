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
        // position          // texCoord   // normal
        -4.0f, 0.0f,  1.5f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
         4.0f, 0.0f,  1.5f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
         4.0f, 0.0f, -31.5f,  1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
        -4.0f, 0.0f, -31.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f
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

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    carpetTexture = loadTexture("carpet.jpeg");
}

void drawScene(GLuint shader) {
    glm::mat4 floorModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &floorModel[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"), 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glUniform1i(glGetUniformLocation(shader, "texture1"), 0);

    glBindVertexArray(floorVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Optional: yellow line (no lighting)
    drawBox(shader, glm::vec3(0.0f, 0.01f, 0.0f), glm::vec3(8.0f, 0.01f, 0.1f), glm::vec3(1.0f, 1.0f, 0.0f));

    // East wall (adjusted Z, full height 0 → 9)
    drawBox(shader, glm::vec3(0.0f, 4.5f, -31.0f), glm::vec3(5.0f, 9.0f, 0.1f), glm::vec3(0.4f));
    drawBox(shader, glm::vec3(-3.125f, 4.5f, -31.0f), glm::vec3(0.75f, 9.0f, 0.1f), glm::vec3(0.2f));
    drawBox(shader, glm::vec3(3.125f, 4.5f, -31.0f), glm::vec3(0.75f, 9.0f, 0.1f), glm::vec3(0.2f));

    // Top 2 rows of windows
    for (int i = 2; i < 4; ++i) {
        float y = 1.3f + i * 2.2f;
        drawBox(shader, glm::vec3(-1.5f, y, -30.95f), glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
        drawBox(shader, glm::vec3(1.5f, y, -30.95f), glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
    }

    // Bottom 2 are doors (extend down to floor)
    for (int i = 0; i < 2; ++i) {
        float y = 1.0f + i * 2.2f;
        drawBox(shader, glm::vec3(-1.5f, y, -30.95f), glm::vec3(1.0f, 2.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
        drawBox(shader, glm::vec3(1.5f, y, -30.95f), glm::vec3(1.0f, 2.0f, 0.05f), glm::vec3(0.6f, 0.8f, 1.0f));
    }
}
