#ifndef COFFEE_MACHINE_HPP
#define COFFEE_MACHINE_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "textured_mesh.hpp"
#include "texture_loader.hpp"

struct CoffeeMachine {
    TexturedMesh base;
    TexturedMesh top;
    TexturedMesh waterTank;
    TexturedMesh controlPanel;
    TexturedMesh dripTray;
};

extern GLuint metalTexture;
extern GLuint redTexture;
extern GLuint blackTexture;
extern GLuint glassTexture;

CoffeeMachine buildCoffeeMachine();
void renderCoffeeMachine(const CoffeeMachine& machine, GLuint shaderProgram, const glm::mat4& modelMatrix);
void cleanupCoffeeMachine(const CoffeeMachine& machine);
TexturedMesh createCoffeeMachineBody();
TexturedMesh createMetalTop();
TexturedMesh createWaterTank();
TexturedMesh createControlPanel();
TexturedMesh createDripTray();

TexturedMesh createSimpleBox(float width, float height, float depth);

#endif
