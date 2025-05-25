// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "shader.hpp"
#include "scene.hpp"
#include "texture_loader.hpp"
#include "sun.hpp"
#include "moon.hpp"
#include "roof.hpp"
#include "ribs.hpp"
#include "dustbin.hpp"

const unsigned int WIDTH = 1000, HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 rotationMatrix = glm::mat4(1.0f);
glm::vec3 rotatedFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0));
glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0));
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + rotatedFront, rotatedUp);

// ========== Point Light Structure ==========
struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;
    float constant;
    float linear;
    float quadratic;
};

// ========== Light Configuration ==========
const int NUM_POINT_LIGHTS = 4;
PointLight wallLights[NUM_POINT_LIGHTS] = {
    // left wall lights (x = -3.9)
    {glm::vec3(-3.9f, 3.0f, -10.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},
    {glm::vec3(-3.9f, 3.0f, -20.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},
    // right wall lights (x = 3.9)
    {glm::vec3(3.9f, 3.0f, -10.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},
    {glm::vec3(3.9f, 3.0f, -20.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f}};

// Day/Night system
bool isDay = true;
bool keyPressed = false; // prevent multiple toggles

// light positions
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

//=======================================================================================================================================
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

    // load main shader & setup scene, then load other shaders
    GLuint shader = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    GLuint roofShader = LoadShaders("roof_vertex.glsl", "roof_fragment.glsl");
    GLuint coverShader = LoadShaders("cover_vertex.glsl", "cover_fragment.glsl");
    GLuint ribsShader = LoadShaders("ribs_vertex.glsl", "ribs_fragment.glsl");
    GLuint sunShader = LoadShaders("sun_vertex.glsl", "sun_fragment.glsl");
    GLuint moonShader = LoadShaders("moon_vertex.glsl", "moon_fragment.glsl");
    GLuint dustbinShader = LoadShaders("dustbin_vertex.glsl", "dustbin_fragment.glsl");

    Sun sun;
    Roof roof(4.0f, 33.0f, 30, 10);
    Ribs ribs(4.0f, 33.0f, 8, 30);
    Moon moon;
    Dustbin dustbin;
    setupScene();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        if (isDay)
        {
            glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // light blue sky
        }
        else
        {
            glClearColor(0.05f, 0.05f, 0.2f, 1.0f); // dark blue sky
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0)), cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

        // Draw main scene
        glUseProgram(shader);
        // NEW: Send point light data to shader
        for (int i = 0; i < NUM_POINT_LIGHTS; i++)
        {
            std::string prefix = "pointLights[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(shader, (prefix + ".position").c_str()), 1, &wallLights[i].position[0]);
            glUniform3fv(glGetUniformLocation(shader, (prefix + ".color").c_str()), 1, &wallLights[i].color[0]);
            glUniform1f(glGetUniformLocation(shader, (prefix + ".constant").c_str()), wallLights[i].constant);
            glUniform1f(glGetUniformLocation(shader, (prefix + ".linear").c_str()), wallLights[i].linear);
            glUniform1f(glGetUniformLocation(shader, (prefix + ".quadratic").c_str()), wallLights[i].quadratic);
        }

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(shader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        drawScene(shader);

        // Draw Roof (Transparent Yellow Semi-Cylinder)
        glUseProgram(roofShader);
        glm::mat4 roofModel = glm::mat4(1.0f);
        roofModel = glm::translate(roofModel, glm::vec3(0.0f, 9.0f, -31.0f)); // start at north wall (Z=1.5)
        roofModel = glm::rotate(roofModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // scale to perfectly match wall dimensions
        roofModel = glm::scale(roofModel, glm::vec3(1.0f, 1.0f, -1.0f)); // ensure proper facing

        glUniformMatrix4fv(glGetUniformLocation(roofShader, "model"), 1, GL_FALSE, glm::value_ptr(roofModel));
        glUniformMatrix4fv(glGetUniformLocation(roofShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(roofShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // set lighting uniforms
        if (isDay)
        {
            glUniform3f(glGetUniformLocation(roofShader, "lightDir"), sunPosition.x, sunPosition.y, sunPosition.z);
            glUniform3f(glGetUniformLocation(roofShader, "lightColor"), 1.0f, 1.0f, 1.0f);
        }
        else
        {
            glUniform3f(glGetUniformLocation(roofShader, "lightDir"), moonPosition.x, moonPosition.y, moonPosition.z);
            glUniform3f(glGetUniformLocation(roofShader, "lightColor"), 0.5f, 0.5f, 1.0f);
        }
        glUniform3f(glGetUniformLocation(roofShader, "objectColor"), 1.0f, 1.0f, 0.0f); // yellow
        roof.DrawRoof();

        // Draw Roof Covers (Black Semi-Circular End Caps)
        glUseProgram(coverShader);
        glUniformMatrix4fv(glGetUniformLocation(coverShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(coverShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // set lighting uniforms for covers
        if (isDay)
        {
            glUniform3f(glGetUniformLocation(coverShader, "lightDir"), sunPosition.x, sunPosition.y, sunPosition.z);
            glUniform3f(glGetUniformLocation(coverShader, "lightColor"), 1.0f, 1.0f, 1.0f);
        }
        else
        {
            glUniform3f(glGetUniformLocation(coverShader, "lightDir"), moonPosition.x, moonPosition.y, moonPosition.z);
            glUniform3f(glGetUniformLocation(coverShader, "lightColor"), 0.5f, 0.5f, 1.0f);
        }
        glUniform3f(glGetUniformLocation(coverShader, "objectColor"), 0.0f, 0.0f, 0.0f); // black

        // Front cover (at the beginning of the roof) WALL WITH 2 ENTRANCE DOORS
        glm::mat4 frontCoverModel = roofModel;
        glUniformMatrix4fv(glGetUniformLocation(coverShader, "model"), 1, GL_FALSE, glm::value_ptr(frontCoverModel));
        roof.DrawCovers();

        // Back cover (at the end of the roof) WALL WITH WINODWS (CLOSER TO ORDER DESK)
        glm::mat4 backCoverModel = roofModel;
        backCoverModel = glm::translate(backCoverModel, glm::vec3(0.0f, 33.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(coverShader, "model"), 1, GL_FALSE, glm::value_ptr(backCoverModel));
        roof.DrawCovers();

        // Draw ribs
        glUseProgram(ribsShader);
        glUniformMatrix4fv(glGetUniformLocation(ribsShader, "model"), 1, GL_FALSE, glm::value_ptr(roofModel));
        glUniformMatrix4fv(glGetUniformLocation(ribsShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ribsShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        ribs.Draw();

        // Draw Dustbin ----------------------------------------
        glUseProgram(dustbinShader);

        // position near west wall (X = -3.5 to leave 0.5 unit space from wall)
        glm::mat4 dustbinModel = glm::mat4(1.0f);
        dustbinModel = glm::translate(dustbinModel, glm::vec3(-3.5f, 1.0f, -30.0f));

        dustbinModel = glm::rotate(dustbinModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));

        glUniformMatrix4fv(glGetUniformLocation(dustbinShader, "model"), 1, GL_FALSE, &dustbinModel[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(dustbinShader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(dustbinShader, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3f(glGetUniformLocation(dustbinShader, "lightPos"),
                    isDay ? sunPosition.x : moonPosition.x,
                    isDay ? sunPosition.y : moonPosition.y,
                    isDay ? sunPosition.z : moonPosition.z);
        glUniform3f(glGetUniformLocation(dustbinShader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        glUniform3f(glGetUniformLocation(dustbinShader, "lightColor"),
                    isDay ? 1.0f : 0.2f,
                    isDay ? 1.0f : 0.2f,
                    isDay ? 1.0f : 0.2f);

        dustbin.render();

        // 2nd dustbin (2 units to the right along the wall)
        glm::mat4 dustbinModel2 = glm::mat4(1.0f);
        dustbinModel2 = glm::translate(dustbinModel2, glm::vec3(-3.5f, 1.0f, -28.0f)); // +2.0 in Z
        dustbinModel2 = glm::rotate(dustbinModel2, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(dustbinShader, "model"), 1, GL_FALSE, &dustbinModel2[0][0]);
        dustbin.render();

        if (isDay)
        {
            // Draw sun
            glUseProgram(sunShader);
            glm::mat4 sunModel = glm::mat4(1.0f);
            sunModel = glm::translate(sunModel, sunPosition);
            sunModel = glm::scale(sunModel, glm::vec3(3.0f));
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
            moonModel = glm::translate(moonModel, moonPosition);
            moonModel = glm::scale(moonModel, glm::vec3(2.0f));
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
