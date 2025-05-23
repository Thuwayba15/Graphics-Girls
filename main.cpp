#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "scene.hpp"
#include "texture_loader.hpp"

const unsigned int WIDTH = 1000, HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 rotationMatrix = glm::mat4(1.0f);
glm::vec3 rotatedFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0));
glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0));
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + rotatedFront, rotatedUp);


void processInput(GLFWwindow* window) {
    float speed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos.y += speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cameraPos.y -= speed;

    float angle = glm::radians(1.5f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0, 1, 0));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, -angle, glm::vec3(0, 1, 0));
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(1, 0, 0));
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, -angle, glm::vec3(1, 0, 0));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0, 0, 1));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        rotationMatrix = glm::rotate(rotationMatrix, -angle, glm::vec3(0, 0, 1));
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics Girls", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    

    GLuint shader = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    glUseProgram(shader);
    setupScene();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.7f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0)), cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);

        glUniform3f(glGetUniformLocation(shader, "lightPos"), 3.0f, 10.0f, 3.0f);
        glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        drawScene(shader);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
