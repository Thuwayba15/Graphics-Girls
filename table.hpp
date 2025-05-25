#ifndef TABLE_HPP
#define TABLE_HPP

#include "textured_mesh.hpp" // Assumes TexturedMesh and related structs are defined here
#include <glm/glm.hpp>

struct Counter {
    TexturedMesh top;
    TexturedMesh legs[4];
    TexturedMesh items[3];
};

void generateCounterTop(TexturedMesh &mesh, float width, float depth, float thickness);
void generateCounterLeg(TexturedMesh &mesh, float height, float width); 
void generateCoffeeCup(TexturedMesh &mesh);
void generateChocolateBar(TexturedMesh &mesh);
void generateCounter(Counter &counter, float width, float depth, float height);
void renderMesh(const TexturedMesh& mesh, const glm::mat4& modelMatrix, GLuint shaderProgram);
void renderCounter(const Counter& counter, GLuint shaderProgram, const glm::mat4& baseModel, float height);
void cleanupMesh(TexturedMesh& mesh);
void cleanupCounter(Counter& counter);


#endif
