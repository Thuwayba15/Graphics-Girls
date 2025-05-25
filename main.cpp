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
#include "textured_mesh.hpp"
#include "sun.hpp"
#include "moon.hpp"
#include "roof.hpp"
#include "ribs.hpp"
#include "dustbin.hpp"
#include "dustbin.hpp"
#include "roundChair.hpp"
#include "highChairs.hpp"
#include "plant.hpp"
#include "cafeCounter.hpp"
#include "coffeeMachine.hpp"
#include "table.hpp"
#include "couch.hpp"



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
const int NUM_POINT_LIGHTS = 8;
PointLight wallLights[NUM_POINT_LIGHTS] = {
    // Corners of the room (near ceiling)
    {glm::vec3(-3.9f, 3.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},   // Front left
    {glm::vec3(3.9f, 3.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},    // Front right
    {glm::vec3(-3.9f, 3.0f, -30.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f}, // Back left
    {glm::vec3(3.9f, 3.0f, -30.0f), glm::vec3(1.0f, 0.8f, 0.6f), 1.0f, 0.09f, 0.032f},  // Back right

    // Optional: extra lights halfway along each wall
    {glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 0.6f, 0.4f), 1.0f, 0.09f, 0.032f},    // Center front
    {glm::vec3(0.0f, 3.0f, -30.0f), glm::vec3(1.0f, 0.6f, 0.4f), 1.0f, 0.09f, 0.032f},  // Center back
    {glm::vec3(-3.9f, 3.0f, -15.0f), glm::vec3(1.0f, 0.6f, 0.4f), 1.0f, 0.09f, 0.032f}, // Mid left
    {glm::vec3(3.9f, 3.0f, -15.0f), glm::vec3(1.0f, 0.6f, 0.4f), 1.0f, 0.09f, 0.032f}   // Mid right
};

// Day/Night system
bool isDay = true;
bool keyPressed = false; // prevent multiple toggles

// light positions
glm::vec3 sunPosition = glm::vec3(3.0f, 25.0f, 3.0f);
glm::vec3 moonPosition = glm::vec3(-4.0f, 25.0f, -3.0f);

// round yellow chairs:
GLuint chairShader;
std::vector<RoundChair> chairs;
const glm::vec3 CHAIR_SCALE = glm::vec3(0.3f); // Proper scale for chair size
const std::vector<glm::vec3> CHAIR_POSITIONS = {
    glm::vec3(1.5f, 0.0f, -10.0f),  // right front
    glm::vec3(-1.5f, 0.0f, -15.0f), // left middle
    glm::vec3(2.0f, 0.0f, -20.0f),  // right back
    glm::vec3(-2.0f, 0.0f, -20.0f)  // left back
};

// high chairs:
GLuint highChairShader;
std::vector<HighChair> highChairs;
const glm::vec3 HIGH_CHAIR_SCALE = glm::vec3(0.4f); // Scale for high chairs
const std::vector<glm::vec3> HIGH_CHAIR_POSITIONS = {
    glm::vec3(-0.5f, 0.0f, -8.0f),  // left front
    glm::vec3(0.5f, 0.0f, -12.0f),  // right middle-front
    glm::vec3(-1.0f, 0.0f, -16.0f), // left middle-back
    glm::vec3(1.2f, 0.0f, -22.0f),  // right back
    glm::vec3(-0.8f, 0.0f, -25.0f)  // left far back
};

// Plant related variables
std::vector<PlantMesh> plants;
std::vector<glm::vec3> plantPositions = {
    glm::vec3(-3.0f, 0.1f, -12.0f),
    glm::vec3(2.0f, 0.1f, -18.0f),
    glm::vec3(-2.0f, 0.1f, -25.0f),
};

// cafeCounter stuff
std::vector<TexturedMesh> counterMeshes;
std::vector<TexturedMesh> chocolateBars;

// coffee machine stuff
CoffeeMachine coffeeMachine;
GLuint shaderProgram;

//Small table
std::vector<Counter> smallTables;

// Couch
std::vector<TexturedMesh> couches;
std::vector<glm::mat4> couchModels;






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

    float speed = 0.3f;
    glm::vec3 rotatedFront = glm::vec3(rotationMatrix * glm::vec4(cameraFront, 0.0));
    glm::vec3 rotatedRight = glm::normalize(glm::cross(rotatedFront, cameraUp));
    glm::vec3 rotatedUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += speed * rotatedFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= speed * rotatedFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= speed * rotatedRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += speed * rotatedRight;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += speed * rotatedUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= speed * rotatedUp;

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
    // round and high chairs:
    chairShader = LoadShaders("roundChair_vertex.glsl", "roundChair_fragment.glsl");   // round yellow chairs
    highChairShader = LoadShaders("highChair_vertex.glsl", "highChair_fragment.glsl"); // high white chairs

    Sun sun;
    Roof roof(4.0f, 33.0f, 30, 10);
    Ribs ribs(4.0f, 33.0f, 8, 30);
    Moon moon;
    Dustbin dustbin;

    // initialize round chairs-----------------------------------------------------------------------------------------------------
    chairs.resize(4);
    for (auto &chair : chairs)
    {
        chair.initialize(chairShader);
    }

    // high chairs
    highChairs.resize(5);
    for (auto &highChair : highChairs)
    {
        highChair.initialize(highChairShader);
    }

    setupScene();

    // Generate plant meshes-----------------------------------------------------------------------------------------------------
    plants.resize(plantPositions.size());
    for (auto &plant : plants)
    {
        auto randomLeaves = generateRandomLeaves(30); // Try 30-50 for lush plants
        generatePlant(plant, randomLeaves);
    }

    // Cafe counter stuff-----------------------------------------------------------------------------------------------------
    createCafeCounter(counterMeshes);
    createChocolateBarsOnCounter(chocolateBars);

    // Generate coffee machine textures
    // Make sure this texture exists

    // Build coffee machine
    metalTexture = generateMetalTexture();
    redTexture = generateSolidColorTexture(0.7f, 0.1f, 0.1f);
    blackTexture = generateSolidColorTexture(0.0f, 0.0f, 0.0f);
    glassTexture = generateGlassTexture(); // This one has alpha!
    coffeeMachine = buildCoffeeMachine();


    // Create small table
    // Create one table per chair
        smallTables.resize(CHAIR_POSITIONS.size());
        for (auto& table : smallTables) {
            generateCounter(table, 0.8f, 0.8f, 0.5f); // Smaller table: width, depth, height
        }

    // Create couches-----------------------------------------------------------------------------------------------------
    // Generate and place 3 couches
    couches.resize(3);
    couchModels.resize(3);

    // Generate all couches
    for (auto& couch : couches) {
        generateCouch(couch, 0.0f, 0.0f, 0.0f, 2.2f, 1.2f, 1.0f, 8); // x,y,z irrelevant here
    }

    // Couch near east wall (facing west)
    couchModels[0] = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -10.0f));
    couchModels[0] = glm::rotate(couchModels[0], glm::radians(180.0f), glm::vec3(0, 1, 0));

    // Couch near west wall (facing east)
    couchModels[1] = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, -20.0f)); // Already facing forward

    // Couch near north wall (facing south)
    couchModels[2] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -29.0f));
    couchModels[2] = glm::rotate(couchModels[2], glm::radians(90.0f), glm::vec3(0, 1, 0));






    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
//-----------------------------------------------------------------------------------------------------
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

        // Draw main scene-----------------------------------------------------------------------------------------------------
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

        // draw round yellow chairs after floor but before transparent objects!!!!!!!!!!!----------------------------------------------------------------------------
        glUseProgram(chairShader);
        for (size_t i = 0; i < chairs.size(); ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, CHAIR_POSITIONS[i]);
            model = glm::scale(model, CHAIR_SCALE);

            // set lighting uniforms to match scene
            glUniform3f(glGetUniformLocation(chairShader, "lightPos"),
                        isDay ? sunPosition.x : moonPosition.x,
                        isDay ? sunPosition.y : moonPosition.y,
                        isDay ? sunPosition.z : moonPosition.z);
            glUniform3f(glGetUniformLocation(chairShader, "viewPos"),
                        cameraPos.x, cameraPos.y, cameraPos.z);
            glUniform3f(glGetUniformLocation(chairShader, "lightColor"),
                        isDay ? 1.0f : 0.2f,
                        isDay ? 1.0f : 0.2f,
                        isDay ? 1.0f : 0.2f);

            chairs[i].render(model, view, projection);
        }

        // Draw high white chairs-----------------------------------------------------------------------------------------------------
        glUseProgram(highChairShader);
        for (size_t i = 0; i < highChairs.size(); ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, HIGH_CHAIR_POSITIONS[i]);
            model = glm::scale(model, HIGH_CHAIR_SCALE);

            // rotate some chairs for variety
            if (i % 2 == 1)
            {
                model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
            }

            glUniform3f(glGetUniformLocation(highChairShader, "lightPos"),
                        isDay ? sunPosition.x : moonPosition.x,
                        isDay ? sunPosition.y : moonPosition.y,
                        isDay ? sunPosition.z : moonPosition.z);
            glUniform3f(glGetUniformLocation(highChairShader, "viewPos"),
                        cameraPos.x, cameraPos.y, cameraPos.z);
            glUniform3f(glGetUniformLocation(highChairShader, "lightColor"),
                        isDay ? 1.0f : 0.3f,
                        isDay ? 1.0f : 0.3f,
                        isDay ? 1.0f : 0.4f);

            highChairs[i].render(model, view, projection);
        }

        // Render plants-----------------------------------------------------------------------------------------------------
        glUseProgram(shader);
        renderPlants(shader, plants, plantPositions);

        // Draw café counter and chocolate bars-----------------------------------------------------------------------------------------------------

        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1); // Enable textures
        glm::mat4 counterModel = glm::mat4(1.0f);
        counterModel = glm::translate(counterModel, glm::vec3(0.0f, 0.0f, -3.0f)); // Adjust Z to be in front of west wall
        // counterModel = glm::rotate(counterModel, glm::radians(180.0f), glm::vec3(0, 1, 0));

        for (const auto &mesh : counterMeshes)
        {
            glActiveTexture(GL_TEXTURE0);                 // Activate texture unit
            glBindTexture(GL_TEXTURE_2D, mesh.textureID); // Bind the texture
            renderTexturedMesh(mesh, shader, counterModel, view, projection);
        }

        renderChocolateBarsOnCounter(chocolateBars, shader, counterModel, view, projection);

        // Position coffee machine on the counter
        glm::mat4 coffeeModel = counterModel;                                   // Reuse the same base as counter
        coffeeModel = glm::translate(coffeeModel, glm::vec3(0.5f, 0.3f, 0.0f)); // Adjust as needed to place properly
        coffeeModel = glm::scale(coffeeModel, glm::vec3(0.3f));

        renderCoffeeMachine(coffeeMachine, shader, coffeeModel);

        //Small tables
        // Use main shader for small tables
        glUseProgram(shader);
        for (size_t i = 0; i < smallTables.size(); ++i)
        {
            glm::vec3 chairPos = CHAIR_POSITIONS[i];
            glm::vec3 tablePos = chairPos + glm::vec3(0.0f, 0.0f, -0.8f); // Offset slightly in front of chair
            glm::mat4 tableModel = glm::mat4(1.0f);
            tableModel = glm::translate(tableModel, tablePos);
            renderCounter(smallTables[i], shader, tableModel, 0.5f);
        }

        // Draw couches-----------------------------------------------------------------------------------------------------
        for (int i = 0; i < 3; ++i) {
            renderMesh(couches[i], couchModels[i], shader);
        }





        



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

        // Draw Dustbin ------------------------------------------------------------------------------------------------------
        glUseProgram(dustbinShader);

        // position near west wall (X = -3.5 to leave 0.5 unit space from wall)
        glm::mat4 dustbinModel = glm::mat4(1.0f);
        dustbinModel = glm::translate(dustbinModel, glm::vec3(-3.5f, 0.5f, -30.0f));
        dustbinModel = glm::rotate(dustbinModel, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        dustbinModel = glm::scale(dustbinModel, glm::vec3(0.7f)); // 70% of original size

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
        dustbinModel2 = glm::translate(dustbinModel2, glm::vec3(-3.5f, 0.5f, -28.0f));
        dustbinModel2 = glm::rotate(dustbinModel2, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        dustbinModel2 = glm::scale(dustbinModel2, glm::vec3(0.7f)); // Same scale factor

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

    // round yellow chairs----------------------------------------------------------------------------------------------------------------------------------------
    for (auto &chair : chairs)
    {
        chair.cleanup();
    }
    for (auto& table : smallTables) {
    cleanupCounter(table);
    }
    glDeleteProgram(chairShader);

    // high chairs
    for (auto &highChair : highChairs)
    {
        highChair.cleanup();
    }
    glDeleteProgram(highChairShader);

    cleanupCoffeeMachine(coffeeMachine);

    glfwTerminate();
    return 0;
}
