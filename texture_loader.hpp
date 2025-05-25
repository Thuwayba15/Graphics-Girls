#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <GL/glew.h>
GLuint loadTexture(const char* filepath);
GLuint generateBrickTexture();
GLuint generateLeatherTexture();
GLuint generateFabricTexture();
GLuint generateMarbleTexture();
GLuint generateMetalTexture();
GLuint generateDarkShinyGreyTexture();
GLuint generateLightGreyMarbleTexture();
GLuint generateSolidColorTexture(float r, float g, float b);
GLuint generateGlassTexture();
GLuint generateCarpetTexture();
GLuint generateWoodTexture();
GLuint generateFloralGlassTexture();





#endif
