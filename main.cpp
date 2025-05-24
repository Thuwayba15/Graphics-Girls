// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "shader.hpp"
#include "scene.hpp"
#include "texture_loader.hpp"
#include "sun.hpp" 
#include "moon.hpp"

const unsigned int WIDTH = 1000, HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 rotationMatrix = glm::mat4(1.0f);
glm::vec3 rotatedFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0));
glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0));
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + rotatedFront, rotatedUp);

// Day/Night system
bool isDay = true;
bool keyPressed = false; // To prevent multiple toggles

// Light positions
glm::vec3 sunPosition = glm::vec3(3.0f, 25.0f, 3.0f);
glm::vec3 moonPosition = glm::vec3(-4.0f, 25.0f, -3.0f);

void processInput(GLFWwindow *window)
{
    // Day/Night toggle with T key
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !keyPressed)
    {
        isDay = !isDay;
        keyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
    {
        keyPressed = false;
    }

    float speed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos.y += speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos.y -= speed;

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

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics Girls", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Load main shader and setup scene
    GLuint shader = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    // Load Sun's shaders
    GLuint sunShader = LoadShaders("sun_vertex.glsl", "sun_fragment.glsl");
    // Load Moon's shaders
    GLuint moonShader = LoadShaders("moon_vertex.glsl", "moon_fragment.glsl");

    // glUseProgram(shader);
    Sun sun; // Create Sun object
    Moon moon;
    setupScene();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        if (isDay)
        {
            glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Light blue sky
        }
        else
        {
            glClearColor(0.05f, 0.05f, 0.2f, 1.0f); // Dark night sky
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0)), cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

        // Draw main scene
        glUseProgram(shader);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(shader, "lightPos"), 3.0f, 10.0f, 3.0f);
        glUniform3f(glGetUniformLocation(shader, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        drawScene(shader);

        if (isDay)
        {
            // Draw sun
            glUseProgram(sunShader);
            glm::mat4 sunModel = glm::mat4(1.0f);
            sunModel = glm::translate(sunModel, sunPosition); // Match lightPos
            sunModel = glm::scale(sunModel, glm::vec3(3.0f)); // Optional: Scale if needed
            glUniformMatrix4fv(glGetUniformLocation(sunShader, "model"), 1, GL_FALSE, glm::value_ptr(sunModel));
            glUniformMatrix4fv(glGetUniformLocation(sunShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(sunShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            sun.Draw();
        }
        else
        {
            // Draw moon
            glUseProgram(moonShader);
            glm::mat4 moonModel = glm::mat4(1.0f);
            moonModel = glm::translate(moonModel, moonPosition); // Match lightPos
            moonModel = glm::scale(moonModel, glm::vec3(2.0f));  // Optional: Scale if needed
            glUniformMatrix4fv(glGetUniformLocation(moonShader, "model"), 1, GL_FALSE, glm::value_ptr(moonModel));
            glUniformMatrix4fv(glGetUniformLocation(moonShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(moonShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            moon.Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
