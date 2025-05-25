#pragma once
#include <GL/glew.h>          
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

struct Leaf {
    float angle;
    float height;
    float size;
    float bend;
};


struct PlantMesh {
    GLuint VAO, VBO, EBO, NBO, TBO;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    GLuint potTexture, stemTexture, soilTexture, leafTexture;
    int potIndexCount = 0;
    int stemIndexCount = 0;
    int leafIndexCount = 0;

    float potShininess, stemShininess, soilShininess, leafShininess;
    glm::vec3 potSpecular, stemSpecular, soilSpecular, leafSpecular;
};

void generatePlant(PlantMesh& mesh, const std::vector<Leaf>& leaves);
void renderPlants(GLuint shader, const std::vector<PlantMesh>& plants, const std::vector<glm::vec3>& positions);

GLuint generatePotTexture();
GLuint generateSoilTexture();
GLuint generateStemTexture();
GLuint generateLeafTexture();
void generatePot(PlantMesh& mesh);
void generateSoil(PlantMesh& mesh);
void generateStem(PlantMesh& mesh);
void generateLeaf(PlantMesh& mesh, float size, float bendAngle);
void generatePlant(PlantMesh& mesh, const std::vector<Leaf>& leaves);
std::vector<Leaf> generateRandomLeaves(int count);




