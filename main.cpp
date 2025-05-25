// main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono> 
#include <vector>
#include <cmath>


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
#include "roof.hpp"
#include "ribs.hpp"

#include <iostream>

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
bool keyPressed = false; // prevent multiple toggles

// light positions
glm::vec3 sunPosition = glm::vec3(3.0f, 25.0f, 3.0f);
glm::vec3 moonPosition = glm::vec3(-4.0f, 25.0f, -3.0f);

// Structure for mesh with texture support
struct TexturedMesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    
    GLuint VAO, VBO, EBO, NBO, TBO;
    GLuint textureID;
    float shininess;
    glm::vec3 specular;
};

struct Counter {
    TexturedMesh top;      // Marble counter top
    TexturedMesh legs[4];  // Metal legs
    TexturedMesh items[3]; // Items on the counter (coffee cups, chocolates, etc.)
};

struct PlantMesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    int potIndicesCount;
    int stemIndicesCount;
    int leavesIndicesCount;

    unsigned int potIndexCount;
    unsigned int stemIndexCount;
    unsigned int leafIndexCount;

    GLuint VAO, VBO, EBO, NBO, TBO;
    
    // Textures for different parts
    GLuint potTexture;
    GLuint soilTexture;
    GLuint stemTexture;
    GLuint leafTexture;
    
    // Material properties
    float potShininess;
    float soilShininess;
    float stemShininess;
    float leafShininess;
    
    glm::vec3 potSpecular;
    glm::vec3 soilSpecular;
    glm::vec3 stemSpecular;
    glm::vec3 leafSpecular;
};

// Generate brick/concrete texture for walls
GLuint generateBrickTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base brick color
            float baseR = 0.7f;
            float baseG = 0.4f;
            float baseB = 0.3f;
            
            // Brick pattern
            int brickHeight = 32;
            int brickWidth = 64;
            int row = y / brickHeight;
            int col = x / brickWidth;
            
            // Offset every other row
            if (row % 2 == 1) {
                col = (x + brickWidth/2) / brickWidth;
            }
            
            // Mortar lines
            bool isVerticalMortar = (x % brickWidth < 4) || (x % brickWidth > brickWidth - 4);
            bool isHorizontalMortar = (y % brickHeight < 4) || (y % brickHeight > brickHeight - 4);
            
            if (isVerticalMortar || isHorizontalMortar) {
                // Mortar color (light gray)
                baseR = 0.8f;
                baseG = 0.8f;
                baseB = 0.75f;
            }
            
            // Add some random variation
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
            
            data[index] = (unsigned char)((baseR + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate marble texture
GLuint generateMarbleTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base marble color (white/cream)
            float baseR = 0.95f;
            float baseG = 0.93f;
            float baseB = 0.90f;
            
            // Create marble veining pattern
            float vein1 = sin((x + y) * 0.02f) * 0.5f + 0.5f;
            float vein2 = sin((x - y) * 0.015f + 2.0f) * 0.5f + 0.5f;
            float vein3 = sin(x * 0.01f + y * 0.025f + 4.0f) * 0.5f + 0.5f;
            
            float veining = (vein1 * vein2 * vein3);
            
            // Add dark veins
            if (veining < 0.3f) {
                baseR *= 0.4f;
                baseG *= 0.4f;
                baseB *= 0.4f;
            } else if (veining < 0.5f) {
                baseR *= 0.7f;
                baseG *= 0.7f;
                baseB *= 0.7f;
            }
            
            // Add subtle noise
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
            
            data[index] = (unsigned char)((baseR + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate carpet texture
GLuint generateCarpetTexture() {
    const int width = 500;  // Increased resolution for better detail
    const int height = 500;
    std::vector<unsigned char> data(width * height * 3);
    
    // Border width in pixels (4 lines along each edge)
    const int borderWidth = 16;
    
    // Base colors
    const float yellowR = 0.8f, yellowG = 0.7f, yellowB = 0.3f;
    const float grayR = 0.4f, grayG = 0.4f, grayB = 0.4f;
    
    // Fluffiness parameters
    const float fiberDensity = 0.6f;  // Higher = more fibers
    const float fiberStrength = 0.5f;  // Higher = more pronounced fibers
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Determine if we're in a border region
            bool isBorder = (x < borderWidth) || (x >= width - borderWidth) ||
                           (y < borderWidth) || (y >= height - borderWidth);
            
            // Base color (yellow for borders, gray otherwise)
            float baseR = isBorder ? yellowR : grayR;
            float baseG = isBorder ? yellowG : grayG;
            float baseB = isBorder ? yellowB : grayB;
            
            // Enhanced fluffy fiber effect
            float fiberPattern = 0.0f;
            
            // Multiple layers of fibers for more complexity
            for (int i = 1; i <= 3; i++) {
                float freq = 0.1f * i;
                float fiber1 = sin(x * freq + y * 0.05f * i) * fiberStrength;
                float fiber2 = cos(y * freq * 0.8f - x * 0.03f * i) * fiberStrength;
                fiberPattern += (fiber1 + fiber2) * fiberDensity;
            }
            
            // Add Perlin-like noise for organic variation
            float noise = 0.0f;
            for (int i = 0; i < 2; i++) {
                float scale = pow(2.0f, i);
                noise += (rand() % 100 / 100.0f) * 0.1f / scale;
            }
            
            // Combine all effects
            float r = baseR + fiberPattern + noise;
            float g = baseG + fiberPattern * 0.7f + noise;
            float b = baseB + fiberPattern * 0.5f + noise;
            
            // Clamp values to [0,1] range
            r = fmax(0.0f, fmin(1.0f, r));
            g = fmax(0.0f, fmin(1.0f, g));
            b = fmax(0.0f, fmin(1.0f, b));
            
            data[index] = (unsigned char)(r * 255);
            data[index + 1] = (unsigned char)(g * 255);
            data[index + 2] = (unsigned char)(b * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}
// Generate metal texture for decorative elements
GLuint generateMetalTexture() {
    const int width = 128;
    const int height = 128;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base metal color (brushed steel)
            float baseR = 0.7f;
            float baseG = 0.7f;
            float baseB = 0.75f;
            
            // Create brushed metal effect
            float brush = sin(y * 0.5f) * 0.1f;
            float scratch = sin(x * 0.1f + y * 0.05f) * 0.05f;
            
            // Add reflective variation
            float reflection = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
            
            data[index] = (unsigned char)((baseR + brush + scratch + reflection) * 255);
            data[index + 1] = (unsigned char)((baseG + brush + scratch + reflection) * 255);
            data[index + 2] = (unsigned char)((baseB + brush + scratch + reflection) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate glass texture with floral pattern
GLuint generateFloralGlassTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 4); // RGBA for transparency
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4;
            
            // Base glass color (slightly tinted)
            data[index] = 240;     // R
            data[index + 1] = 245; // G
            data[index + 2] = 250; // B
            
            // Create floral pattern
            float flowerAlpha = 0.0f;
            
            // Multiple flower centers
            for (int f = 0; f < 8; f++) {
                float fx = width * (0.2f + (f % 3) * 0.3f);
                float fy = height * (0.2f + (f / 3) * 0.3f);
                float dist = sqrt(pow(x - fx, 2) + pow(y - fy, 2));
                
                // Flower petals
                if (dist < 40.0f && dist > 10.0f) {
                    float angle = atan2(y - fy, x - fx);
                    float petal = sin(angle * 3.0f) * 0.5f + 0.5f; // 6 petals
                    if (petal > 0.3f) {
                        flowerAlpha = std::max(flowerAlpha, 0.6f * (1.0f - dist / 40.0f));
                    }
                }
                
                // Flower center
                if (dist < 12.0f) {
                    flowerAlpha = std::max(flowerAlpha, 0.8f);
                }
            }
            
            data[index + 3] = (unsigned char)(flowerAlpha * 255); // Alpha
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Simple solid color texture generator
GLuint generateSolidColorTexture(float r, float g, float b) {
    const int width = 4;
    const int height = 4;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int i = 0; i < width * height * 3; i += 3) {
        data[i] = (unsigned char)(r * 255);
        data[i + 1] = (unsigned char)(g * 255);
        data[i + 2] = (unsigned char)(b * 255);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate procedural fabric texture
GLuint generateFabricTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 3,255);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            //pure white base colour
            float baseR = 1.0f;
            float baseG = 1.0f;
            float baseB = 1.0f;

            // Add fabric weave pattern
            float weaveX = sin(x * 0.2f) * 0.1f;
            float weaveY = sin(y * 0.2f) * 0.1f;
            float weave = (weaveX + weaveY) * 0.2f;
            
            // Add some random variation
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.02f;
               // Thread pattern
            float thread = 0.0f;
            if ((x % 40 < 20 && y % 40 < 20) || (x % 40 >= 20 && y % 40 >= 20)) {
                thread = 0.03f;
            }
            data[index] = (unsigned char)((baseR + weave + thread + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + weave + thread + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + weave + thread + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

GLuint generateLeatherTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 3);
    
    // Base leather color (brown)
    float baseR = 0.4f;
    float baseG = 0.2f;
    float baseB = 0.1f;
    
    // Create leather texture with wrinkles and pores
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Wrinkles
            float wrinkleX = sin(x * 0.01f) * 0.05f;
            float wrinkleY = sin(y * 0.015f) * 0.05f;
            float wrinkles = (wrinkleX + wrinkleY) * 0.5f;
            
            // Pores
            float pore = 0.0f;
            if ((x % 20 < 18 && y % 20 < 18) || (x % 20 >= 18 && y % 20 >= 18)) {
                pore = 0.1f;
            }
            
            // Add some random variation
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
            
            data[index] = (unsigned char)((baseR + wrinkles + pore + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + wrinkles * 0.8f + pore * 0.7f + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + wrinkles * 0.6f + pore * 0.5f + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}
// Generate wood texture
GLuint generateWoodTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 3);
    
    // Base wood color
    float baseR = 0.5f;
    float baseG = 0.3f;
    float baseB = 0.2f;
    
    // Create wood grain pattern
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Wood grain pattern
            float grain = sin(x * 0.01f + y * 0.005f) * 0.2f;
            
            // Add some knots
            float knot = 0.0f;
            for (int k = 0; k < 5; k++) {
                float kx = width * (0.2f + k * 0.15f);
                float ky = height * (0.2f + k * 0.15f);
                float dist = sqrt(pow(x - kx, 2) + pow(y - ky, 2));
                if (dist < 30.0f) {
                    knot = 0.1f * (1.0f - dist / 30.0f);
                }
            }
            
            // Add some random variation
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
            
            data[index] = (unsigned char)((baseR + grain + knot + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + grain * 0.8f + knot * 0.7f + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + grain * 0.6f + knot * 0.5f + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate glass texture with floral pattern
GLuint generateGlassTexture() {
    const int width = 512;
    const int height = 512;
    std::vector<unsigned char> data(width * height * 4); // RGBA for transparency
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4;
            
            // Base glass color (slightly tinted)
            data[index] = 240;     // R
            data[index + 1] = 245; // G
            data[index + 2] = 250; // B
            
            // Create floral pattern
            float flowerAlpha = 0.0f;
            
            // Multiple flower centers
            for (int f = 0; f < 8; f++) {
                float fx = width * (0.2f + (f % 3) * 0.3f);
                float fy = height * (0.2f + (f / 3) * 0.3f);
                float dist = sqrt(pow(x - fx, 2) + pow(y - fy, 2));
                
                // Flower petals
                if (dist < 40.0f && dist > 10.0f) {
                    float angle = atan2(y - fy, x - fx);
                    float petal = sin(angle * 3.0f) * 0.5f + 0.5f; // 6 petals
                    if (petal > 0.3f) {
                        flowerAlpha = std::max(flowerAlpha, 0.6f * (1.0f - dist / 40.0f));
                    }
                }
                
                // Flower center
                if (dist < 12.0f) {
                    flowerAlpha = std::max(flowerAlpha, 0.8f);
                }
            }
            
            data[index + 3] = (unsigned char)(flowerAlpha * 255); // Alpha
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate black pot texture
GLuint generatePotTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base black color with slight variation
            float baseColor = 0.1f;
            float variation = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
            float finalColor = baseColor + variation;
            
            // Add subtle ceramic shine pattern
            float shine = 0.0f;
            if ((x + y) % 20 < 2) {
                shine = 0.02f;
            }
            
            finalColor += shine;
            finalColor = std::max(0.0f, std::min(1.0f, finalColor));
            
            data[index] = (unsigned char)(finalColor * 255);
            data[index + 1] = (unsigned char)(finalColor * 255);
            data[index + 2] = (unsigned char)(finalColor * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate soil texture
GLuint generateSoilTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base brown color
            float baseR = 0.35f;
            float baseG = 0.25f;
            float baseB = 0.15f;
            
            // Add granular texture
            float grain = ((rand() % 100) / 100.0f - 0.5f) * 0.2f;
            // Organic matter (darker spots)
            float organic = 0.0f;
            if ((x % 15 < 8 && y % 15 < 8) || (rand() % 100 < 5)) {
                organic = -0.08f;
            }
            
            // Small rocks (lighter spots)
            float rocks = 0.0f;
            if (rand() % 200 < 3) {
                rocks = 0.15f;
            }
            
            data[index] = (unsigned char)((baseR + grain + organic + rocks) * 255);
            data[index + 1] = (unsigned char)((baseG + grain * 0.8f + organic * 0.7f + rocks * 0.8f) * 255);
            data[index + 2] = (unsigned char)((baseB + grain * 0.6f + organic * 0.5f + rocks * 0.6f) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate leaf texture
GLuint generateLeafTexture() {
    const int width = 128;
    const int height = 128;
    std::vector<unsigned char> data(width * height * 4); // RGBA for transparency
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4;
            
            // Base green color
            float baseR = 0.1f + ((rand() % 100) / 1000.0f);
            float baseG = 0.6f + ((rand() % 100) / 500.0f);
            float baseB = 0.2f + ((rand() % 100) / 1000.0f);
            
            // Create leaf shape (elliptical)
            float centerX = width / 2.0f;
            float centerY = height / 2.0f;
            float distX = (x - centerX) / centerX;
            float distY = (y - centerY) / centerY;
            float leafShape = 1.0f - (distX * distX + distY * distY);
            
            // Vein pattern - main central vein
            float centralVein = 1.0f - abs(distX) * 4.0f;
            centralVein = std::max(0.0f, std::min(1.0f, centralVein));
            
            // Side veins
            float sideVeins = 0.0f;
            if (abs(sin(distX * 10.0f + distY * 3.0f)) > 0.9f) {
                sideVeins = 0.1f;
            }
            
            // Apply leaf shape as alpha
            float alpha = std::max(0.0f, leafShape);
            
            data[index] = (unsigned char)((baseR + centralVein * 0.1f + sideVeins) * 255);
            data[index + 1] = (unsigned char)((baseG + centralVein * 0.15f + sideVeins) * 255);
            data[index + 2] = (unsigned char)((baseB + centralVein * 0.05f + sideVeins) * 255);
            data[index + 3] = (unsigned char)(alpha * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

GLuint generateStemTexture() {
    const int width = 128;
    const int height = 128;
    std::vector<unsigned char> data(width * height * 3);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base stem color (brownish-green)
            float baseR = 0.25f;
            float baseG = 0.35f;
            float baseB = 0.15f;
            
            // Vertical fiber pattern
            float fiber = 0.5f + 0.3f * sin(x * 0.3f);
            
            // Horizontal growth rings
            float rings = 0.8f + 0.2f * sin(y * 0.1f);
            
            // Random texture
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
            
            data[index] = (unsigned char)((baseR * fiber * rings + noise) * 255);
            data[index + 1] = (unsigned char)((baseG * fiber * rings + noise) * 255);
            data[index + 2] = (unsigned char)((baseB * fiber * rings + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

struct Leaf {
    float angle;
    float height;
    float size;
    float bend;
};

const int NUM_LEAVES = 30; // Number of leaves to generate


void generatePot(PlantMesh& mesh) {
    const float potRadius = 0.8f;
    const float potHeight = 1.0f;
    const int segments = 32;
    const int rings = 12;
    
    int potStartIndex = mesh.vertices.size() / 3;
    
    // Generate pot walls (tapered cylinder)
    for (int i = 0; i <= rings; i++) {
        float y = -potHeight / 2.0f + (float)i / rings * potHeight;
        float radius = potRadius - (potRadius - 0.6f) * (float)i / rings;
        
        for (int j = 0; j <= segments; j++) {
            float angle = 2.0f * M_PI * (float)j / segments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);
            
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(y);
            mesh.vertices.push_back(z);
            
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
            mesh.normals.push_back(normal.x);
            mesh.normals.push_back(normal.y);
            mesh.normals.push_back(normal.z);
            
            mesh.texCoords.push_back((float)j / segments);
            mesh.texCoords.push_back((float)i / rings);
        }
    }
    
    // Generate indices for pot walls
    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int first = potStartIndex + i * (segments + 1) + j;
            int second = first + segments + 1;
            
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);
            
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }
    
    // Generate pot bottom
    int bottomStartIndex = mesh.vertices.size() / 3;
    float bottomY = -potHeight / 2.0f;
    
    // Center vertex for bottom
    mesh.vertices.push_back(0.0f);
    mesh.vertices.push_back(bottomY);
    mesh.vertices.push_back(0.0f);
    mesh.normals.push_back(0.0f);
    mesh.normals.push_back(-1.0f);
    mesh.normals.push_back(0.0f);
    mesh.texCoords.push_back(0.5f);
    mesh.texCoords.push_back(0.5f);
    
    // Outer vertices for bottom
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * (float)i / segments;
        float x = potRadius * cos(angle);
        float z = potRadius * sin(angle);
        
        mesh.vertices.push_back(x);
        mesh.vertices.push_back(bottomY);
        mesh.vertices.push_back(z);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(-1.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back(0.5f + 0.5f * cos(angle));
        mesh.texCoords.push_back(0.5f + 0.5f * sin(angle));
    }
    
    // Generate indices for pot bottom
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(bottomStartIndex);
        mesh.indices.push_back(bottomStartIndex + i + 2);
        mesh.indices.push_back(bottomStartIndex + i + 1);
    }
}

void generateStem(PlantMesh& mesh) {
    const float height = 1.8f;
    const float baseRadius = 0.04f;
    const float topRadius = 0.02f;
    const int segments = 12;
    const int rings = 20;
    
    int stemStartIndex = mesh.vertices.size() / 3;
    
    // Generate stem vertices starting from soil level
    for (int i = 0; i <= rings; i++) {
        float y = 0.35f + (float)i / rings * height; // Start from soil level
        float radius = baseRadius + (topRadius - baseRadius) * (float)i / rings;
        
        // Add slight curve to make it more natural
        float curve = sin((float)i / rings * M_PI * 0.5f) * 0.05f;
        
        for (int j = 0; j <= segments; j++) {
            float angle = 2.0f * M_PI * (float)j / segments;
            float x = radius * cos(angle) + curve;
            float z = radius * sin(angle);
            
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(y);
            mesh.vertices.push_back(z);
            
            glm::vec3 normal = glm::normalize(glm::vec3(x - curve, 0.0f, z));
            mesh.normals.push_back(normal.x);
            mesh.normals.push_back(normal.y);
            mesh.normals.push_back(normal.z);
            
            mesh.texCoords.push_back((float)j / segments);
            mesh.texCoords.push_back((float)i / rings);
        }
    }
    
    // Generate indices for stem
    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int first = stemStartIndex + i * (segments + 1) + j;
            int second = first + segments + 1;
            
            mesh.indices.push_back(first);
            mesh.indices.push_back(second);
            mesh.indices.push_back(first + 1);
            
            mesh.indices.push_back(second);
            mesh.indices.push_back(second + 1);
            mesh.indices.push_back(first + 1);
        }
    }
}

void generateSoil(PlantMesh& mesh) {
    const float soilRadius = 0.58f; // Smaller than pot to fit inside
    const float soilHeight = 0.35f; // Position inside pot, near top
    const int segments = 32;
    
    int soilStartIndex = mesh.vertices.size() / 3;
    
    // Center vertex
    mesh.vertices.push_back(0.0f);
    mesh.vertices.push_back(soilHeight);
    mesh.vertices.push_back(0.0f);
    mesh.normals.push_back(0.0f);
    mesh.normals.push_back(1.0f);
    mesh.normals.push_back(0.0f);
    mesh.texCoords.push_back(0.5f);
    mesh.texCoords.push_back(0.5f);
    
    // Outer vertices with slight height variation for realistic soil surface
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * (float)i / segments;
        float x = soilRadius * cos(angle);
        float z = soilRadius * sin(angle);
        float heightVariation = ((rand() % 100) / 1000.0f - 0.05f); // Small random height variation
        
        mesh.vertices.push_back(x);
        mesh.vertices.push_back(soilHeight + heightVariation);
        mesh.vertices.push_back(z);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(1.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back(0.5f + 0.5f * cos(angle));
        mesh.texCoords.push_back(0.5f + 0.5f * sin(angle));
    }
    
    // Generate indices for soil surface
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(soilStartIndex);
        mesh.indices.push_back(soilStartIndex + i + 1);
        mesh.indices.push_back(soilStartIndex + ((i + 1) % segments) + 1);
    }
}

void generateLeaf(PlantMesh& mesh, float size, float bendAngle) {
    // Create a more realistic leaf shape
    const int segments = 8;
    int leafStartIndex = mesh.vertices.size() / 3;
    
    // Create leaf vertices in a more natural oval shape
    std::vector<glm::vec3> leafPoints;
    
    // Generate leaf outline points
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float angle = t * 2.0f * M_PI;
        
        // Create oval shape with pointed tip
        float radiusX = size * 0.9f * (1.0f - t * 0.3f);
        float radiusY = size * (t < 0.5f ? t * 2.0f : (1.0f - t) * 2.0f);
        
        float x = radiusX * cos(angle);
        float y = radiusY;
        float z = 0.0f;
        
        // Apply bend
        float bendRad = glm::radians(bendAngle);
        x = x * cos(bendRad);
        z = x * sin(bendRad);
        
        leafPoints.push_back(glm::vec3(x, y, z));
    }
    
    // Create center point
    glm::vec3 center(0.0f, size * 0.3f, 0.0f);
    
    // Add vertices
    mesh.vertices.push_back(center.x);
    mesh.vertices.push_back(center.y);
    mesh.vertices.push_back(center.z);
    mesh.normals.push_back(0.0f);
    mesh.normals.push_back(0.0f);
    mesh.normals.push_back(1.0f);
    mesh.texCoords.push_back(0.5f);
    mesh.texCoords.push_back(0.5f);
    
    for (const auto& point : leafPoints) {
        mesh.vertices.push_back(point.x);
        mesh.vertices.push_back(point.y);
        mesh.vertices.push_back(point.z);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(1.0f);
        mesh.texCoords.push_back((point.x / size + 1.0f) * 0.5f);
        mesh.texCoords.push_back(point.y / size);
    }
    
    // Generate indices for triangular fan
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(leafStartIndex);
        mesh.indices.push_back(leafStartIndex + i + 1);
        mesh.indices.push_back(leafStartIndex + ((i + 1) % segments) + 1);
    }
}

void generatePlant(PlantMesh& mesh, const std::vector<Leaf>& leaves) {
    // Clear existing data
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.texCoords.clear();
    mesh.indices.clear();

    // Generate textures
    mesh.potTexture = generatePotTexture();
    mesh.soilTexture = generateSoilTexture();
    mesh.stemTexture = generateStemTexture();
    mesh.leafTexture = generateLeafTexture();

    // Set material properties
    mesh.potShininess = 64.0f;
    mesh.soilShininess = 2.0f;
    mesh.stemShininess = 16.0f;
    mesh.leafShininess = 32.0f;
    
    mesh.potSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
    mesh.soilSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
    mesh.stemSpecular = glm::vec3(0.2f, 0.3f, 0.1f);
    mesh.leafSpecular = glm::vec3(0.3f, 0.5f, 0.3f);

    // Generate components
    generatePot(mesh);
    mesh.potIndexCount = mesh.indices.size();
    generateSoil(mesh);
    generateStem(mesh);
    mesh.stemIndexCount = mesh.indices.size() - mesh.potIndexCount;
    
    // Generate leaves at different heights and angles
    for (const auto& leaf : leaves) {
        int baseIndex = mesh.vertices.size() / 3;
        
        generateLeaf(mesh, leaf.size, leaf.bend);
        
        // Transform leaf vertices
        for (int i = baseIndex; i < mesh.vertices.size() / 3; i++) {
            glm::vec3 vertex(mesh.vertices[i*3], mesh.vertices[i*3+1], mesh.vertices[i*3+2]);
            glm::vec3 normal(mesh.normals[i*3], mesh.normals[i*3+1], mesh.normals[i*3+2]);
            
            // Rotate around Y axis
            float angleRad = glm::radians(leaf.angle);
            glm::mat3 rotation = glm::mat3(
                cos(angleRad), 0, sin(angleRad),
                0, 1, 0,
                -sin(angleRad), 0, cos(angleRad)
            );
            
            vertex = rotation * vertex;
            normal = rotation * normal;
            
            // Position at correct height on stem
            vertex.y += leaf.height;
            
            mesh.vertices[i*3] = vertex.x;
            mesh.vertices[i*3+1] = vertex.y;
            mesh.vertices[i*3+2] = vertex.z;
            
            mesh.normals[i*3] = normal.x;
            mesh.normals[i*3+1] = normal.y;
            mesh.normals[i*3+2] = normal.z;
        }
    }
    mesh.leafIndexCount = mesh.indices.size() - (mesh.potIndexCount + mesh.stemIndexCount);
    
    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}
// Structure to store generated mesh data

void renderPlants(GLuint shaderProgram, const std::vector<PlantMesh>& plants, const std::vector<glm::vec3>& plantPositions,  const glm::mat4& view, const glm::mat4& projection) {
    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, plantPositions[i]);
        
        // Set uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(plants[i].VAO);
        
        // Render pot
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), plants[i].potShininess);
        glUniform3fv(glGetUniformLocation(shaderProgram, "specular"), 1, glm::value_ptr(plants[i].potSpecular));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plants[i].potTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTexture"), 0);
        glDrawElements(GL_TRIANGLES, plants[i].potIndexCount, GL_UNSIGNED_INT, 0);
        
        // Render soil
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), plants[i].soilShininess);
        glUniform3fv(glGetUniformLocation(shaderProgram, "specular"), 1, glm::value_ptr(plants[i].soilSpecular));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plants[i].soilTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
        glDrawElements(GL_TRIANGLES, plants[i].stemIndexCount, GL_UNSIGNED_INT, (void*)(plants[i].potIndexCount * sizeof(unsigned int)));
        
        // Render stem
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), plants[i].stemShininess);
        glUniform3fv(glGetUniformLocation(shaderProgram, "specular"), 1, glm::value_ptr(plants[i].stemSpecular));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plants[i].stemTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
        glDrawElements(GL_TRIANGLES, plants[i].stemIndexCount, GL_UNSIGNED_INT, (void*)(plants[i].potIndexCount * sizeof(unsigned int)));

        
        // Render leaves
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), plants[i].leafShininess);
        glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(plants[i].leafSpecular));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plants[i].leafTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
        int leafStartIndex = plants[i].potIndexCount + plants[i].stemIndexCount;
        glDrawElements(GL_TRIANGLES, plants[i].leafIndexCount, GL_UNSIGNED_INT, (void*)(leafStartIndex * sizeof(unsigned int)));
    }
    
    glBindVertexArray(0);
}


struct Mesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
};

// Function to generate a couch cushion with tufting effects
void generateCushion(TexturedMesh &mesh, float x, float y, float z, float width, float height, float depth, int segments) {
    float xStart = x - width / 2.0f;
    float zStart = z - depth / 2.0f;
    
    // Create vertices for the top surface of the cushion with a slight depression and tufting
    int baseIndex = mesh.vertices.size() / 3;
    
    // Define tufting button positions (in normalized coordinates)
    const int numTuftingButtons = 9; // 3x3 grid of buttons
    glm::vec2 buttonPositions[numTuftingButtons] = {
        glm::vec2(0.25f, 0.25f), glm::vec2(0.5f, 0.25f), glm::vec2(0.75f, 0.25f),
        glm::vec2(0.25f, 0.5f), glm::vec2(0.5f, 0.5f), glm::vec2(0.75f, 0.5f),
        glm::vec2(0.25f, 0.75f), glm::vec2(0.5f, 0.75f), glm::vec2(0.75f, 0.75f)
    };
    
    // Grid of vertices for top surface
    for (int i = 0; i <= segments; i++) {
        for (int j = 0; j <= segments; j++) {
            float xPos = xStart + (width * i) / segments;
            float zPos = zStart + (depth * j) / segments;
            
            // Normalize coordinates for calculations
            float normalizedX = (float)i / segments;
            float normalizedZ = (float)j / segments;
            
            // Calculate basic depression for cushion effect (edge bulging)
            float distX = 2.0f * (normalizedX - 0.5f);
            float distZ = 2.0f * (normalizedZ - 0.5f);
            float edgeEffect = 0.05f * height * (distX * distX + distZ * distZ);
            
            // Add tufting effect (buttons that create depressions)
            float tuftingDepression = 0.0f;
            for (int b = 0; b < numTuftingButtons; b++) {
                float buttonDistX = normalizedX - buttonPositions[b].x;
                float buttonDistZ = normalizedZ - buttonPositions[b].y;
                float buttonDist = std::sqrt(buttonDistX * buttonDistX + buttonDistZ * buttonDistZ);
                
                // Create depression around button with smooth falloff
                if (buttonDist < 0.1f) {
                    float buttonEffect = 0.15f * height * (1.0f - buttonDist / 0.1f);
                    tuftingDepression += buttonEffect;
                }
            }
            
            // Combine effects for final cushion shape
            float totalDepression = edgeEffect + tuftingDepression;
            float yPos = y + height - totalDepression;
            
            // Add small random displacement for more natural look
            float randomDisplacement = ((rand() % 100) / 1000.0f) * height * 0.02f;
            yPos -= randomDisplacement;
            
            // Add vertex position
            mesh.vertices.push_back(xPos);
            mesh.vertices.push_back(yPos);
            mesh.vertices.push_back(zPos);
            
            // Calculate normal vector based on surface derivatives
            // This creates better lighting effects on the curved cushion surface
            float dydx = 0.0f;
            float dydz = 0.0f;
            
            // Edge bulge derivatives
            dydx += 0.2f * height * distX;
            dydz += 0.2f * height * distZ;
            
            // Tufting derivatives
            for (int b = 0; b < numTuftingButtons; b++) {
                float buttonDistX = normalizedX - buttonPositions[b].x;
                float buttonDistZ = normalizedZ - buttonPositions[b].y;
                float buttonDist = std::sqrt(buttonDistX * buttonDistX + buttonDistZ * buttonDistZ);
                
                if (buttonDist < 0.1f && buttonDist > 0.001f) {
                    float factor = 1.5f * height * (1.0f / buttonDist - 10.0f);
                    dydx -= factor * buttonDistX / buttonDist;
                    dydz -= factor * buttonDistZ / buttonDist;
                }
            }
            
            // Convert derivatives to normal vector
            float nx = -dydx;
            float ny = 1.0f;
            float nz = -dydz;
            
            // Normalize the normal vector
            float len = std::sqrt(nx*nx + ny*ny + nz*nz);
            mesh.normals.push_back(nx / len);
            mesh.normals.push_back(ny / len);
            mesh.normals.push_back(nz / len);
        }
    }
    
    // Create indices for the top triangles
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int topLeft = baseIndex + i * (segments + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (segments + 1);
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            
            // Second triangle
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }
    
    // Create bottom vertices
    int bottomBaseIndex = mesh.vertices.size() / 3;
    for (int i = 0; i <= segments; i++) {
        for (int j = 0; j <= segments; j++) {
            float xPos = xStart + (width * i) / segments;
            float zPos = zStart + (depth * j) / segments;
            float yPos = y;
            
            // Add vertex position
            mesh.vertices.push_back(xPos);
            mesh.vertices.push_back(yPos);
            mesh.vertices.push_back(zPos);
            
            // Add normal vector (down)
            mesh.normals.push_back(0.0f);
            mesh.normals.push_back(-1.0f);
            mesh.normals.push_back(0.0f);

            //Texture coordinates
            mesh.texCoords.push_back((float)i / segments);
            mesh.texCoords.push_back((float)j / segments);
        }
    }
    
    // Create indices for the bottom triangles
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int topLeft = bottomBaseIndex + i * (segments + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (segments + 1);
            int bottomRight = bottomLeft + 1;
            
            // First triangle (reversed winding)
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            
            // Second triangle (reversed winding)
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomRight);
            mesh.indices.push_back(bottomLeft);
        }
    }
    
    // Create the sides
    // Front side
    int frontBaseIndex = mesh.vertices.size() / 3;
    for (int i = 0; i <= segments; i++) {
        float xPos = xStart + (width * i) / segments;
        
        // Top vertex
        float distX = 2.0f * (i / (float)segments - 0.5f);
        float distZ = 2.0f * (0.0f - 0.5f);
        float depression = 0.08f * height * (1.0f - (distX * distX + distZ * distZ));
        depression = std::max(depression, 0.0f);
        
        float yPosTop = y + height - depression;
        float zPosTop = zStart;
        
        mesh.vertices.push_back(xPos);
        mesh.vertices.push_back(yPosTop);
        mesh.vertices.push_back(zPosTop);
        
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(-1.0f);
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(0.0f);
        
        // Bottom vertex
        float yPosBottom = y;
        mesh.vertices.push_back(xPos);
        mesh.vertices.push_back(yPosBottom);
        mesh.vertices.push_back(zPosTop);
        
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(-1.0f);
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(1.0f);
    }
    
    // Create indices for the front side
    for (int i = 0; i < segments; i++) {
        int topLeft = frontBaseIndex + i * 2;
        int bottomLeft = topLeft + 1;
        int topRight = topLeft + 2;
        int bottomRight = topRight + 1;
        
        // First triangle
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(topRight);
        
        // Second triangle
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(bottomRight);
    }
    
    // Back side
    int backBaseIndex = mesh.vertices.size() / 3;
    for (int i = 0; i <= segments; i++) {
        float xPos = xStart + (width * i) / segments;
        
        // Top vertex
        float distX = 2.0f * (i / (float)segments - 0.5f);
        float distZ = 2.0f * (1.0f - 0.5f);
        float depression = 0.08f * height * (1.0f - (distX * distX + distZ * distZ));
        depression = std::max(depression, 0.0f);
        
        float yPosTop = y + height - depression;
        float zPosTop = zStart + depth;
        
        mesh.vertices.push_back(xPos);
        mesh.vertices.push_back(yPosTop);
        mesh.vertices.push_back(zPosTop);
        
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(1.0f);
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(0.0f);
        
        // Bottom vertex
        float yPosBottom = y;
        mesh.vertices.push_back(xPos);
        mesh.vertices.push_back(yPosBottom);
        mesh.vertices.push_back(zPosTop);
        
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(1.0f);
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(1.0f);
    }
    
    // Create indices for the back side
    for (int i = 0; i < segments; i++) {
        int topLeft = backBaseIndex + i * 2;
        int bottomLeft = topLeft + 1;
        int topRight = topLeft + 2;
        int bottomRight = topRight + 1;
        
        // First triangle (reversed winding)
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomLeft);
        
        // Second triangle (reversed winding)
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomRight);
        mesh.indices.push_back(bottomLeft);
    }
    
    // Left side
    int leftBaseIndex = mesh.vertices.size() / 3;
    for (int j = 0; j <= segments; j++) {
        float zPos = zStart + (depth * j) / segments;
        
        // Top vertex
        float distX = 2.0f * (0.0f - 0.5f);
        float distZ = 2.0f * (j / (float)segments - 0.5f);
        float depression = 0.08f * height * (1.0f - (distX * distX + distZ * distZ));
        depression = std::max(depression, 0.0f);
        
        float yPosTop = y + height - depression;
        float xPosLeft = xStart;
        
        mesh.vertices.push_back(xPosLeft);
        mesh.vertices.push_back(yPosTop);
        mesh.vertices.push_back(zPos);
        
        mesh.normals.push_back(-1.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back((float)j / segments);
        mesh.texCoords.push_back(0.0f);

        
        // Bottom vertex
        float yPosBottom = y;
        mesh.vertices.push_back(xPosLeft);
        mesh.vertices.push_back(yPosBottom);
        mesh.vertices.push_back(zPos);
        
        mesh.normals.push_back(-1.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back((float)j / segments);
        mesh.texCoords.push_back(1.0f);
    }
    
    // Create indices for the left side
    for (int j = 0; j < segments; j++) {
        int topLeft = leftBaseIndex + j * 2;
        int bottomLeft = topLeft + 1;
        int topRight = topLeft + 2;
        int bottomRight = topRight + 1;
        
        // First triangle
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(topRight);
        
        // Second triangle
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(bottomRight);
    }
    
    // Right side
    int rightBaseIndex = mesh.vertices.size() / 3;
    for (int j = 0; j <= segments; j++) {
        float zPos = zStart + (depth * j) / segments;
        
        // Top vertex
        float distX = 2.0f * (1.0f - 0.5f);
        float distZ = 2.0f * (j / (float)segments - 0.5f);
        float depression = 0.08f * height * (1.0f - (distX * distX + distZ * distZ));
        depression = std::max(depression, 0.0f);
        
        float yPosTop = y + height - depression;
        float xPosRight = xStart + width;
        
        mesh.vertices.push_back(xPosRight);
        mesh.vertices.push_back(yPosTop);
        mesh.vertices.push_back(zPos);
        
        mesh.normals.push_back(1.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back((float)j / segments);
        mesh.texCoords.push_back(0.0f);
        
        // Bottom vertex
        float yPosBottom = y;
        mesh.vertices.push_back(xPosRight);
        mesh.vertices.push_back(yPosBottom);
        mesh.vertices.push_back(zPos);
        
        mesh.normals.push_back(1.0f);
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(0.0f);
        mesh.texCoords.push_back((float)j / segments);
        mesh.texCoords.push_back(1.0f);
    }
    
    // Create indices for the right side
    for (int j = 0; j < segments; j++) {
        int topLeft = rightBaseIndex + j * 2;
        int bottomLeft = topLeft + 1;
        int topRight = topLeft + 2;
        int bottomRight = topRight + 1;
        
        // First triangle (reversed winding)
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomLeft);
        
        // Second triangle (reversed winding)
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomRight);
        mesh.indices.push_back(bottomLeft);
    }

    //Generate texture
    mesh.textureID = generateFabricTexture();
    mesh.shininess = 25.0f;
    mesh.specular = glm::vec3(0.3, 0.3, 0.3);

    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

// Function to generate a rectangular box for couch frame
void generateBox(TexturedMesh &mesh, float x, float y, float z, float width, float height, float depth) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;
    
    // Vertex positions (8 corners of a box)
    std::vector<float> positions = {
        // Front face
        x - halfWidth, y - halfHeight, z + halfDepth, // bottom left
        x + halfWidth, y - halfHeight, z + halfDepth, // bottom right
        x + halfWidth, y + halfHeight, z + halfDepth, // top right
        x - halfWidth, y + halfHeight, z + halfDepth, // top left
        
        // Back face
        x - halfWidth, y - halfHeight, z - halfDepth, // bottom left
        x + halfWidth, y - halfHeight, z - halfDepth, // bottom right
        x + halfWidth, y + halfHeight, z - halfDepth, // top right
        x - halfWidth, y + halfHeight, z - halfDepth  // top left
    };
    
    // Normal vectors for each face
    std::vector<float> normals = {
        // Front face (z+)
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        
        // Back face (z-)
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
        // Left face (x-)
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        
        // Right face (x+)
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        // Top face (y+)
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        
        // Bottom face (y-)
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };
    
    // Texture coordinates
    std::vector<float> texCoords = {
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    

    // Store the starting index for our new vertices
    unsigned int baseIndex = mesh.vertices.size() / 3;
    
    // Add vertices and normals to the mesh
    for (size_t i = 0; i < positions.size(); i += 3) {
        mesh.vertices.push_back(positions[i]);
        mesh.vertices.push_back(positions[i+1]);
        mesh.vertices.push_back(positions[i+2]);
        
        mesh.normals.push_back(normals[i]);
        mesh.normals.push_back(normals[i+1]);
        mesh.normals.push_back(normals[i+2]);
    }

    for (size_t i = 0; i < texCoords.size(); i += 2) {
        mesh.texCoords.push_back(texCoords[i]);
        mesh.texCoords.push_back(texCoords[i+1]);
    }
    
    // Define indices for the box faces
    std::vector<unsigned int> boxIndices = {
        // Front face
        baseIndex + 0, baseIndex + 1, baseIndex + 2,
        baseIndex + 2, baseIndex + 3, baseIndex + 0,
        
        // Back face
        baseIndex + 5, baseIndex + 4, baseIndex + 7,
        baseIndex + 7, baseIndex + 6, baseIndex + 5,
        
        // Left face
        baseIndex + 4, baseIndex + 0, baseIndex + 3,
        baseIndex + 3, baseIndex + 7, baseIndex + 4,
        
        // Right face
        baseIndex + 1, baseIndex + 5, baseIndex + 6,
        baseIndex + 6, baseIndex + 2, baseIndex + 1,
        
        // Top face
        baseIndex + 3, baseIndex + 2, baseIndex + 6,
        baseIndex + 6, baseIndex + 7, baseIndex + 3,
        
        // Bottom face
        baseIndex + 4, baseIndex + 5, baseIndex + 1,
        baseIndex + 1, baseIndex + 0, baseIndex + 4
    };
    
    // Add the new indices to the mesh
    for (unsigned int index : boxIndices) {
        mesh.indices.push_back(index);
    }

    // Generate wood texture for the box
    mesh.textureID = generateWoodTexture();
    mesh.shininess = 64.0f;
    mesh.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    
    // Additional vertices and normals for box edges
    int numSides = 8;
    unsigned int edgeBaseIndex = mesh.vertices.size() / 3;
   
        // Create OpenGL buffers if they don't exist
    if (mesh.VAO == 0) {
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);
        glGenBuffers(1, &mesh.NBO);
        glGenBuffers(1, &mesh.TBO);
    }
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void generateArmrest(TexturedMesh &mesh, float x, float y, float z, float width, float height, float depth, int segments) {
    // Base of the armrest
    generateBox(mesh, x, y + height/2, z, width, height, depth);
    
    // Add a cushion on top
    TexturedMesh cushion;
    generateCushion(cushion, x, y + height, z, width, height * 0.3f, depth, segments);
    
    // Merge the cushion into the main mesh
    unsigned int baseIndex = mesh.vertices.size() / 3;
    for (size_t i = 0; i < cushion.vertices.size(); i++) {
        mesh.vertices.push_back(cushion.vertices[i]);
    }
    for (size_t i = 0; i < cushion.normals.size(); i++) {
        mesh.normals.push_back(cushion.normals[i]);
    }
    for (size_t i = 0; i < cushion.texCoords.size(); i++) {
        mesh.texCoords.push_back(cushion.texCoords[i]);
    }
    for (size_t i = 0; i < cushion.indices.size(); i++) {
        mesh.indices.push_back(cushion.indices[i] + baseIndex);
    }
}

void generateBackrest(TexturedMesh &mesh, float x, float y, float z, float width, float height, float depth, int segments) {
    // Base of the backrest
    generateBox(mesh, x, y + height/2, z, width, height, depth);
    
    // Add a series of cushions on the front
    float cushionWidth = width / 3.0f;
    float cushionHeight = height * 0.3f;
    float cushionDepth = depth * 0.4f;
    
    for (int i = 0; i < 3; i++) {
        float cushionX = x - width/2 + cushionWidth/2 + i * cushionWidth;
        float cushionY = y + height - cushionHeight;
        float cushionZ = z + depth/2 - cushionDepth/2;
        
        TexturedMesh cushion;
        generateCushion(cushion, cushionX, cushionY, cushionZ, cushionWidth * 0.9f, cushionHeight, cushionDepth, segments);
        
        // Merge the cushion into the main mesh
        unsigned int baseIndex = mesh.vertices.size() / 3;
        for (size_t i = 0; i < cushion.vertices.size(); i++) {
            mesh.vertices.push_back(cushion.vertices[i]);
        }
        for (size_t i = 0; i < cushion.normals.size(); i++) {
            mesh.normals.push_back(cushion.normals[i]);
        }
        for (size_t i = 0; i < cushion.texCoords.size(); i++) {
            mesh.texCoords.push_back(cushion.texCoords[i]);
        }
        for (size_t i = 0; i < cushion.indices.size(); i++) {
            mesh.indices.push_back(cushion.indices[i] + baseIndex);
        }
    }
}


void generateCouch(TexturedMesh &mesh, float x, float y, float z, float width, float height, float depth, int segments) {
    float baseHeight = height * 0.3f;
    float seatHeight = height * 0.4f;
    float backHeight = height;
    float armWidth = width * 0.15f;
    float cushionDepth = depth * 0.8f;
    
    // Base frame
    generateBox(mesh, x, y + baseHeight/2, z, width, baseHeight, depth);
    
    // Seat cushions (3 across)
    float cushionWidth = (width - 2 * armWidth) / 3.0f;
    for (int i = 0; i < 3; i++) {
        float cushionX = x - width/2 + armWidth + cushionWidth/2 + i * cushionWidth;
        float cushionY = y + baseHeight;
        float cushionZ = z;
        
        TexturedMesh cushion;
        generateCushion(cushion, cushionX, cushionY, cushionZ, cushionWidth * 0.95f, seatHeight, cushionDepth, segments);
        
        // Merge the cushion into the main mesh
        unsigned int baseIndex = mesh.vertices.size() / 3;
        for (size_t i = 0; i < cushion.vertices.size(); i++) {
            mesh.vertices.push_back(cushion.vertices[i]);
        }
        for (size_t i = 0; i < cushion.normals.size(); i++) {
            mesh.normals.push_back(cushion.normals[i]);
        }
        for (size_t i = 0; i < cushion.texCoords.size(); i++) {
            mesh.texCoords.push_back(cushion.texCoords[i]);
        }
        for (size_t i = 0; i < cushion.indices.size(); i++) {
            mesh.indices.push_back(cushion.indices[i] + baseIndex);
        }
    }
    
    // Arm rests
    // Left armrest
    TexturedMesh leftArm;
    generateArmrest(leftArm, x - width/2 + armWidth/2, y + baseHeight, z, armWidth, seatHeight, depth, segments);
    
    // Merge left armrest into main mesh
    unsigned int baseIndex = mesh.vertices.size() / 3;
    for (size_t i = 0; i < leftArm.vertices.size(); i++) {
        mesh.vertices.push_back(leftArm.vertices[i]);
    }
    for (size_t i = 0; i < leftArm.normals.size(); i++) {
        mesh.normals.push_back(leftArm.normals[i]);
    }
    for (size_t i = 0; i < leftArm.texCoords.size(); i++) {
        mesh.texCoords.push_back(leftArm.texCoords[i]);
    }
    for (size_t i = 0; i < leftArm.indices.size(); i++) {
        mesh.indices.push_back(leftArm.indices[i] + baseIndex);
    }
    
    // Right armrest
    TexturedMesh rightArm;
    generateArmrest(rightArm, x + width/2 - armWidth/2, y + baseHeight, z, armWidth, seatHeight, depth, segments);
    
    // Merge right armrest into main mesh
    baseIndex = mesh.vertices.size() / 3;
    for (size_t i = 0; i < rightArm.vertices.size(); i++) {
        mesh.vertices.push_back(rightArm.vertices[i]);
    }
    for (size_t i = 0; i < rightArm.normals.size(); i++) {
        mesh.normals.push_back(rightArm.normals[i]);
    }
    for (size_t i = 0; i < rightArm.texCoords.size(); i++) {
        mesh.texCoords.push_back(rightArm.texCoords[i]);
    }
    for (size_t i = 0; i < rightArm.indices.size(); i++) {
        mesh.indices.push_back(rightArm.indices[i] + baseIndex);
    }
    
    // Backrest
    TexturedMesh backrest;
    generateBackrest(backrest, x, y + baseHeight, z - depth/2 + depth*0.15f, width, backHeight, depth * 0.3f, segments);
    
    // Merge backrest into main mesh
    baseIndex = mesh.vertices.size() / 3;
    for (size_t i = 0; i < backrest.vertices.size(); i++) {
        mesh.vertices.push_back(backrest.vertices[i]);
    }
    for (size_t i = 0; i < backrest.normals.size(); i++) {
        mesh.normals.push_back(backrest.normals[i]);
    }
    for (size_t i = 0; i < backrest.texCoords.size(); i++) {
        mesh.texCoords.push_back(backrest.texCoords[i]);
    }
    for (size_t i = 0; i < backrest.indices.size(); i++) {
        mesh.indices.push_back(backrest.indices[i] + baseIndex);
    }
    
    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

//counter
// Generic mesh rendering function

void generateCounterTop(TexturedMesh &mesh, float width, float depth, float thickness) {
    // Clear any existing data
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.texCoords.clear();
    mesh.indices.clear();

    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;

    // Vertices for the counter top (a flat box)
    std::vector<float> positions = {
        // Top face
        -halfWidth, thickness, -halfDepth,
         halfWidth, thickness, -halfDepth,
         halfWidth, thickness,  halfDepth,
        -halfWidth, thickness,  halfDepth,
        
        // Bottom face
        -halfWidth, 0.0f, -halfDepth,
         halfWidth, 0.0f, -halfDepth,
         halfWidth, 0.0f,  halfDepth,
        -halfWidth, 0.0f,  halfDepth,
        
        // Side faces
        -halfWidth, 0.0f, -halfDepth,
        -halfWidth, thickness, -halfDepth,
        -halfWidth, thickness, halfDepth,
        -halfWidth, 0.0f, halfDepth,
        
        halfWidth, 0.0f, -halfDepth,
        halfWidth, thickness, -halfDepth,
        halfWidth, thickness, halfDepth,
        halfWidth, 0.0f, halfDepth,
        
        -halfWidth, 0.0f, -halfDepth,
        -halfWidth, thickness, -halfDepth,
        halfWidth, thickness, -halfDepth,
        halfWidth, 0.0f, -halfDepth,
        
        -halfWidth, 0.0f, halfDepth,
        -halfWidth, thickness, halfDepth,
        halfWidth, thickness, halfDepth,
        halfWidth, 0.0f, halfDepth
    };

    // Normals
    std::vector<float> normals = {
        // Top face
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        
        // Bottom face
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        
        // Left face
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        
        // Right face
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        // Front face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        
        // Back face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    // Texture coordinates
    std::vector<float> texCoords = {
        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Indices
    std::vector<unsigned int> indices = {
        // Top face
        0, 1, 2,
        2, 3, 0,
        
        // Bottom face
        4, 5, 6,
        6, 7, 4,
        
        // Left face
        8, 9, 10,
        10, 11, 8,
        
        // Right face
        12, 13, 14,
        14, 15, 12,
        
        // Front face
        16, 17, 18,
        18, 19, 16,
        
        // Back face
        20, 21, 22,
        22, 23, 20
    };

    // Assign to mesh
    mesh.vertices = positions;
    mesh.normals = normals;
    mesh.texCoords = texCoords;
    mesh.indices = indices;

    // Generate marble texture
    mesh.textureID = generateMarbleTexture();
    mesh.shininess = 64.0f;
    mesh.specular = glm::vec3(0.3f, 0.3f, 0.3f);

    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void generateCounterLeg(TexturedMesh &mesh, float height, float width) {
    // Clear any existing data
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.texCoords.clear();
    mesh.indices.clear();

    float halfWidth = 0.05;

    // Vertices for a simple square leg
    std::vector<float> positions = {
        // Front face
        -halfWidth, 0.0f, -halfWidth,
         halfWidth, 0.0f, -halfWidth,
         halfWidth, height, -halfWidth,
        -halfWidth, height, -halfWidth,
        
        // Back face
        -halfWidth, 0.0f, halfWidth,
         halfWidth, 0.0f, halfWidth,
         halfWidth, height, halfWidth,
        -halfWidth, height, halfWidth,
        
        // Left face
        -halfWidth, 0.0f, -halfWidth,
        -halfWidth, height, -halfWidth,
        -halfWidth, height, halfWidth,
        -halfWidth, 0.0f, halfWidth,
        
        // Right face
        halfWidth, 0.0f, -halfWidth,
        halfWidth, height, -halfWidth,
        halfWidth, height, halfWidth,
        halfWidth, 0.0f, halfWidth,
        
        // Top face
        -halfWidth, height, -halfWidth,
         halfWidth, height, -halfWidth,
         halfWidth, height, halfWidth,
        -halfWidth, height, halfWidth,
        
        // Bottom face
        -halfWidth, 0.0f, -halfWidth,
         halfWidth, 0.0f, -halfWidth,
         halfWidth, 0.0f, halfWidth,
        -halfWidth, 0.0f, halfWidth
    };

    // Normals
    std::vector<float> normals = {
        // Front face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        
        // Back face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        
        // Left face
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        
        // Right face
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        // Top face
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        
        // Bottom face
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };

    // Texture coordinates
    std::vector<float> texCoords = {
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Indices
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        
        // Back face
        4, 5, 6,
        6, 7, 4,
        
        // Left face
        8, 9, 10,
        10, 11, 8,
        
        // Right face
        12, 13, 14,
        14, 15, 12,
        
        // Top face
        16, 17, 18,
        18, 19, 16,
        
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // Assign to mesh
    mesh.vertices = positions;
    mesh.normals = normals;
    mesh.texCoords = texCoords;
    mesh.indices = indices;

    // Generate metal texture
    mesh.textureID = generateMetalTexture();
    mesh.shininess = 128.0f;
    mesh.specular = glm::vec3(0.8f, 0.8f, 0.8f);

    // Create OpenGL buffers (same as counter top)
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void generateCoffeeCup(TexturedMesh &mesh) {
    // Clear any existing data
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.texCoords.clear();
    mesh.indices.clear();

    const int segments = 20;
    const float height = 0.1f;
    const float bottomRadius = 0.1f;
    const float topRadius = 0.125f;
    
    // Generate vertices for the cup (a cylinder)
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * (float)i / segments;
        float x = cos(angle);
        float z = sin(angle);
        
        // Bottom ring
        mesh.vertices.push_back(x * bottomRadius);
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(z * bottomRadius);
        
        glm::vec3 bottomNormal = glm::normalize(glm::vec3(x, 0.0f, z));
        mesh.normals.push_back(bottomNormal.x);
        mesh.normals.push_back(bottomNormal.y);
        mesh.normals.push_back(bottomNormal.z);
        
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(0.0f);
        
        // Top ring
        mesh.vertices.push_back(x * topRadius);
        mesh.vertices.push_back(height);
        mesh.vertices.push_back(z * topRadius);
        
        glm::vec3 topNormal = glm::normalize(glm::vec3(x, 0.2f, z));
        mesh.normals.push_back(topNormal.x);
        mesh.normals.push_back(topNormal.y);
        mesh.normals.push_back(topNormal.z);
        
        mesh.texCoords.push_back((float)i / segments);
        mesh.texCoords.push_back(1.0f);
    }
    
    // Generate indices for the sides
    for (int i = 0; i < segments; i++) {
        int bottomLeft = i * 2;
        int topLeft = bottomLeft + 1;
        int bottomRight = bottomLeft + 2;
        int topRight = topLeft + 2;
        
        mesh.indices.push_back(bottomLeft);
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(bottomRight);
        
        mesh.indices.push_back(topLeft);
        mesh.indices.push_back(topRight);
        mesh.indices.push_back(bottomRight);
    }
    
    // Generate bottom
    int centerIndex = mesh.vertices.size() / 3;
    mesh.vertices.push_back(0.0f);
    mesh.vertices.push_back(0.0f);
    mesh.vertices.push_back(0.0f);
    
    mesh.normals.push_back(0.0f);
    mesh.normals.push_back(-1.0f);
    mesh.normals.push_back(0.0f);
    
    mesh.texCoords.push_back(0.5f);
    mesh.texCoords.push_back(0.5f);
    
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * (float)i / segments;
        float x = cos(angle) * bottomRadius;
        float z = sin(angle) * bottomRadius;
        
        mesh.vertices.push_back(x);
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(z);
        
        mesh.normals.push_back(0.0f);
        mesh.normals.push_back(-1.0f);
        mesh.normals.push_back(0.0f);
        
        mesh.texCoords.push_back(0.5f + 0.5f * cos(angle));
        mesh.texCoords.push_back(0.5f + 0.5f * sin(angle));
    }
    
    // Generate indices for bottom
    for (int i = 0; i < segments; i++) {
        mesh.indices.push_back(centerIndex);
        mesh.indices.push_back(centerIndex + i + 1);
        mesh.indices.push_back(centerIndex + ((i + 1) % segments) + 1);
    }
    
    // Generate handle
    // (Simple torus segment for the handle)
    const int handleSegments = 5;
    const float handleRadius = 0.025f;
    const float handleDistance = 0.15f;
    
    int handleBaseIndex = mesh.vertices.size() / 3;
    for (int i = 0; i <= handleSegments; i++) {
        float angle = M_PI * (float)i / handleSegments;
        for (int j = 0; j <= 1; j++) {
            float x = handleDistance + cos(angle) * handleRadius;
            float y = height * 0.5f + sin(angle) * handleRadius;
            float z = (j == 0) ? -handleRadius : handleRadius;
            
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(y);
            mesh.vertices.push_back(z);
            
            glm::vec3 normal = glm::normalize(glm::vec3(cos(angle), sin(angle), 0.0f));
            mesh.normals.push_back(normal.x);
            mesh.normals.push_back(normal.y);
            mesh.normals.push_back(normal.z);
            
            mesh.texCoords.push_back((float)i / handleSegments);
            mesh.texCoords.push_back((float)j);
        }
    }
    
    // Generate indices for handle
    for (int i = 0; i < handleSegments; i++) {
        int leftBottom = handleBaseIndex + i * 2;
        int leftTop = leftBottom + 1;
        int rightBottom = leftBottom + 2;
        int rightTop = leftTop + 2;
        
        mesh.indices.push_back(leftBottom);
        mesh.indices.push_back(rightBottom);
        mesh.indices.push_back(leftTop);
        
        mesh.indices.push_back(leftTop);
        mesh.indices.push_back(rightBottom);
        mesh.indices.push_back(rightTop);
    }
    
    // Generate texture (white ceramic)
    mesh.textureID = generateSolidColorTexture(0.95f, 0.95f, 0.95f);
    mesh.shininess = 32.0f;
    mesh.specular = glm::vec3(0.3f, 0.3f, 0.3f);

    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void generateChocolateBar(TexturedMesh &mesh) {
    // Clear any existing data
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.texCoords.clear();
    mesh.indices.clear();

    float width = 0.15f;
    float height = 0.05f;
    float depth = 0.08f;
    
    // Simple rectangular prism
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    // Vertices
    std::vector<float> positions = {
        // Front face
        -halfWidth, -halfHeight, halfDepth,
         halfWidth, -halfHeight, halfDepth,
         halfWidth,  halfHeight, halfDepth,
        -halfWidth,  halfHeight, halfDepth,
        
        // Back face
        -halfWidth, -halfHeight, -halfDepth,
         halfWidth, -halfHeight, -halfDepth,
         halfWidth,  halfHeight, -halfDepth,
        -halfWidth,  halfHeight, -halfDepth,
        
        // Left face
        -halfWidth, -halfHeight, -halfDepth,
        -halfWidth,  halfHeight, -halfDepth,
        -halfWidth,  halfHeight, halfDepth,
        -halfWidth, -halfHeight, halfDepth,
        
        // Right face
        halfWidth, -halfHeight, -halfDepth,
        halfWidth,  halfHeight, -halfDepth,
        halfWidth,  halfHeight, halfDepth,
        halfWidth, -halfHeight, halfDepth,
        
        // Top face
        -halfWidth, halfHeight, -halfDepth,
         halfWidth, halfHeight, -halfDepth,
         halfWidth, halfHeight, halfDepth,
        -halfWidth, halfHeight, halfDepth,
        
        // Bottom face
        -halfWidth, -halfHeight, -halfDepth,
         halfWidth, -halfHeight, -halfDepth,
         halfWidth, -halfHeight, halfDepth,
        -halfWidth, -halfHeight, halfDepth
    };

    // Normals
    std::vector<float> normals = {
        // Front face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        
        // Left face
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        
        // Right face
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        // Top face
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        
        // Bottom face
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };

    // Texture coordinates
    std::vector<float> texCoords = {
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // Indices
    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        
        // Back face
        4, 5, 6,
        6, 7, 4,
        
        // Left face
        8, 9, 10,
        10, 11, 8,
        
        // Right face
        12, 13, 14,
        14, 15, 12,
        
        // Top face
        16, 17, 18,
        18, 19, 16,
        
        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

    // Assign to mesh
    mesh.vertices = positions;
    mesh.normals = normals;
    mesh.texCoords = texCoords;
    mesh.indices = indices;

    // Generate brown texture
    mesh.textureID = generateSolidColorTexture(0.4f, 0.2f, 0.1f);
    mesh.shininess = 16.0f;
    mesh.specular = glm::vec3(0.1f, 0.1f, 0.1f);

    // Create OpenGL buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);
    glGenBuffers(1, &mesh.NBO);
    glGenBuffers(1, &mesh.TBO);
    
    glBindVertexArray(mesh.VAO);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Texture coordinates buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void generateCounter(Counter &counter, float width, float depth, float height) {
    // Generate counter top
    generateCounterTop(counter.top, width, depth, 0.1f);
    
    // Generate legs (position them at the corners)
    float legOffsetX = width / 2.0f - 0.15f;
    float legOffsetZ = depth / 2.0f - 0.15f;
    float legHeight = height*0.35f; // Subtract counter top thickness
    
    // Front left leg
    generateCounterLeg(counter.legs[0], legHeight, 0.1f);
    
    // Front right leg
    generateCounterLeg(counter.legs[1], legHeight, 0.1f);
    
    // Back left leg
    generateCounterLeg(counter.legs[2], legHeight, 0.1f);
    
    // Back right leg
    generateCounterLeg(counter.legs[3], legHeight, 0.1f);
    
    // Generate items on the counter
    generateCoffeeCup(counter.items[0]);
    generateChocolateBar(counter.items[1]);

    generateChocolateBar(counter.items[2]); // Example for another item
    // You can add more items here
}

void renderMesh(const TexturedMesh& mesh, const glm::mat4& modelMatrix, GLuint shaderProgram) {
    // Set model matrix uniform
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    // Set material properties
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), mesh.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(mesh.specular));
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    
    // Render the mesh
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void renderCounter(const Counter& counter, GLuint shaderProgram, const glm::mat4& baseModel) {
    // Render counter top
    glm::mat4 topModel = baseModel; // Start with the base transformation (includes rotation)
    topModel = glm::translate(topModel, glm::vec3(0.0f, 0.8f, 0.0f)); // Position at counter height
    renderMesh(counter.top, topModel, shaderProgram);
    
    // Render legs at corners
    float legOffsetX = 2.0f / 2.0f - 0.15f; // Half width minus leg inset
    float legOffsetZ = 1.0f / 2.0f - 0.15f; // Half depth minus leg inset
    float legHeight = 0.8f - 0.1f; // Counter height minus top thickness
    
    // Front left leg
    glm::mat4 leg1Model = baseModel; // Start with the base transformation
    leg1Model = glm::translate(leg1Model, glm::vec3(-legOffsetX, legHeight/2.0f, -legOffsetZ));
    renderMesh(counter.legs[0], leg1Model, shaderProgram);
    
    // Front right leg
    glm::mat4 leg2Model = baseModel; // Start with the base transformation
    leg2Model = glm::translate(leg2Model, glm::vec3(legOffsetX, legHeight/2.0f, -legOffsetZ));
    renderMesh(counter.legs[1], leg2Model, shaderProgram);
    
    // Back left leg
    glm::mat4 leg3Model = baseModel; // Start with the base transformation
    leg3Model = glm::translate(leg3Model, glm::vec3(-legOffsetX, legHeight/2.0f, legOffsetZ));
    renderMesh(counter.legs[2], leg3Model, shaderProgram);
    
    // Back right leg
    glm::mat4 leg4Model = baseModel; // Start with the base transformation
    leg4Model = glm::translate(leg4Model, glm::vec3(legOffsetX, legHeight/2.0f, legOffsetZ));
    renderMesh(counter.legs[3], leg4Model, shaderProgram);
    
    // Render items on counter
    // Coffee cup
    glm::mat4 cupModel = baseModel; // Start with the base transformation
    cupModel = glm::translate(cupModel, glm::vec3(-0.5f, 0.9f, 0.2f)); // On top of counter
    renderMesh(counter.items[0], cupModel, shaderProgram);
    
    // Chocolate bar
    glm::mat4 chocolateModel = baseModel; // Start with the base transformation
    chocolateModel = glm::translate(chocolateModel, glm::vec3(0.3f, 0.85f, -0.1f)); // On top of counter
    renderMesh(counter.items[1], chocolateModel, shaderProgram);
}
// Generic mesh cleanup
void cleanupMesh(TexturedMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
    glDeleteBuffers(1, &mesh.NBO);
    glDeleteBuffers(1, &mesh.TBO);
    glDeleteTextures(1, &mesh.textureID);
}
// Cleanup function
void cleanupCounter(Counter& counter) {
    // Clean up counter top
    cleanupMesh(counter.top);
    
    // Clean up legs
    for (int i = 0; i < 4; i++) {
        cleanupMesh(counter.legs[i]);
    }
    
    // Clean up items
    for (int i = 0; i < 3; i++) {
        cleanupMesh(counter.items[i]);
    }
}

//carpet
GLuint createCarpetVAO(float width, float length, float thickness) {
    // Vertices for the carpet (a flat rectangle with slight thickness)
    float vertices[] = {
        // Positions          // Normals         // Texture coords
        // Top face
        -width/2, 0.0f, -length/2,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         width/2, 0.0f, -length/2,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         width/2, 0.0f,  length/2,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -width/2, 0.0f,  length/2,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        
        // Bottom face
        -width/2, -thickness, -length/2,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         width/2, -thickness, -length/2,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         width/2, -thickness,  length/2,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -width/2, -thickness,  length/2,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        
        // Side faces
        -width/2, 0.0f, -length/2,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -width/2, -thickness, -length/2,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -width/2, -thickness,  length/2,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -width/2, 0.0f,  length/2,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        
         width/2, 0.0f, -length/2,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         width/2, -thickness, -length/2,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         width/2, -thickness,  length/2,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         width/2, 0.0f,  length/2,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        
        -width/2, 0.0f, -length/2,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -width/2, -thickness, -length/2,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
         width/2, -thickness, -length/2,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
         width/2, 0.0f, -length/2,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
        
        -width/2, 0.0f,  length/2,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        -width/2, -thickness,  length/2,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         width/2, -thickness,  length/2,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
         width/2, 0.0f,  length/2,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        // Top face
        0, 1, 2,
        2, 3, 0,
        
        // Bottom face
        4, 5, 6,
        6, 7, 4,
        
        // Side faces
        8, 9, 10,
        10, 11, 8,
        
        12, 13, 14,
        14, 15, 12,
        
        16, 17, 18,
        18, 19, 16,
        
        20, 21, 22,
        22, 23, 20
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return VAO;
}

void renderCarpet(GLuint shaderProgram, GLuint carpetVAO, GLuint carpetTexture) {
    glUseProgram(shaderProgram);
    
    // Set texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, carpetTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    
    // Draw carpet
    glBindVertexArray(carpetVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


std::vector<Leaf> generateRandomLeaves(int count) {
    std::vector<Leaf> leaves;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> angleDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> heightDist(0.5f, 2.1f);
    std::uniform_real_distribution<float> sizeDist(0.3f, 0.7f);
    std::uniform_real_distribution<float> bendDist(-20.0f, 20.0f);

    for (int i = 0; i < count; i++) {
        leaves.push_back({
            angleDist(gen),    // Random angle
            heightDist(gen),   // Random height
            sizeDist(gen),    // Random size
            bendDist(gen)      // Random bend
        });
    }
    return leaves;
}

//Counter marble
// Generate light grey marble texture for counter top
GLuint generateLightGreyMarbleTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base light grey marble color
            float baseR = 0.85f;
            float baseG = 0.85f;
            float baseB = 0.85f;
            
        // Create marble veining pattern (more veins)
            float vein1 = sin((x + y) * 0.03f) * 0.5f + 0.5f;
            float vein2 = sin((x - y) * 0.025f + 2.0f) * 0.5f + 0.5f;
            float vein3 = sin(x * 0.02f + y * 0.035f + 4.0f) * 0.5f + 0.5f;
            float vein4 = sin((x + y * 2) * 0.018f + 1.5f) * 0.5f + 0.5f;
            float vein5 = sin((x * 3 - y) * 0.012f + 3.5f) * 0.5f + 0.5f;
            float vein6 = sin(x * 0.008f + y * 0.045f + 6.0f) * 0.5f + 0.5f;
            
            float veining = (vein1 * vein2 * vein3 * vein4 * vein5 * vein6);
            
            // Create lighter streaks pattern
            float streak1 = sin(x * 0.05f + y * 0.01f) * 0.5f + 0.5f;
            float streak2 = sin((x + y) * 0.04f + 1.0f) * 0.5f + 0.5f;
            float lightStreaks = streak1 * streak2;
            
            // Add darker grey veins for marble pattern
            if (veining < 0.3f) {
                baseR *= 0.6f;  // Darker grey veins
                baseG *= 0.6f;
                baseB *= 0.6f;
            } else if (veining < 0.5f) {
                baseR *= 0.8f;  // Medium grey veins
                baseG *= 0.8f;
                baseB *= 0.8f;
            }

             // Add lighter streaks
            if (lightStreaks > 0.8f) {
                baseR = std::min(1.0f, baseR * 1.3f);  // Lighter streaks
                baseG = std::min(1.0f, baseG * 1.3f);
                baseB = std::min(1.0f, baseB * 1.3f);
            }
             
            // Add subtle noise for texture
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.03f;
            
            data[index] = (unsigned char)((baseR + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

// Generate dark shiny grey texture for counter base
GLuint generateDarkShinyGreyTexture() {
    const int width = 256;
    const int height = 256;
    std::vector<unsigned char> data(width * height * 3);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            
            // Base very dark grey color
            float baseR = 0.15f;
            float baseG = 0.15f;
            float baseB = 0.15f;
            
            // Create subtle wood grain or brushed metal pattern
            float pattern1 = sin(x * 0.05f) * 0.1f + 0.9f;
            float pattern2 = sin(y * 0.03f + 1.5f) * 0.05f + 0.95f;
            
            float patternCombined = pattern1 * pattern2;
            
            // Apply pattern for subtle variation
            baseR *= patternCombined;
            baseG *= patternCombined;
            baseB *= patternCombined;
            
            // Add very subtle noise for realistic texture
            float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.03f;
            
            data[index] = (unsigned char)((baseR + noise) * 255);
            data[index + 1] = (unsigned char)((baseG + noise) * 255);
            data[index + 2] = (unsigned char)((baseB + noise) * 255);
        }
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}


// Function to calculate normals for a mesh
void calculateNormals(TexturedMesh& mesh) {
    mesh.normals.clear();
    mesh.normals.resize(mesh.vertices.size(), 0.0f);
    
    // Calculate face normals and accumulate vertex normals
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        unsigned int i0 = mesh.indices[i] * 3;
        unsigned int i1 = mesh.indices[i + 1] * 3;
        unsigned int i2 = mesh.indices[i + 2] * 3;
        
        // Get triangle vertices
        glm::vec3 v0(mesh.vertices[i0], mesh.vertices[i0 + 1], mesh.vertices[i0 + 2]);
        glm::vec3 v1(mesh.vertices[i1], mesh.vertices[i1 + 1], mesh.vertices[i1 + 2]);
        glm::vec3 v2(mesh.vertices[i2], mesh.vertices[i2 + 1], mesh.vertices[i2 + 2]);
        
        // Calculate face normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        // Accumulate normal for each vertex of the triangle
        mesh.normals[i0] += normal.x;
        mesh.normals[i0 + 1] += normal.y;
        mesh.normals[i0 + 2] += normal.z;
        
        mesh.normals[i1] += normal.x;
        mesh.normals[i1 + 1] += normal.y;
        mesh.normals[i1 + 2] += normal.z;
        
        mesh.normals[i2] += normal.x;
        mesh.normals[i2 + 1] += normal.y;
        mesh.normals[i2 + 2] += normal.z;
    }
    
    // Normalize all vertex normals
    for (size_t i = 0; i < mesh.normals.size(); i += 3) {
        glm::vec3 normal(mesh.normals[i], mesh.normals[i + 1], mesh.normals[i + 2]);
        normal = glm::normalize(normal);
        mesh.normals[i] = normal.x;
        mesh.normals[i + 1] = normal.y;
        mesh.normals[i + 2] = normal.z;
    }
}

// Function to setup OpenGL buffers for a textured mesh
void setupMeshBuffers(TexturedMesh& mesh) {
    // Generate and bind VAO
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);
    
    // Setup vertex buffer (VBO)
    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Setup normal buffer (NBO)
    glGenBuffers(1, &mesh.NBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Setup texture coordinate buffer (TBO)
    glGenBuffers(1, &mesh.TBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Setup element buffer (EBO)
    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    // Unbind VAO
    glBindVertexArray(0);
}

// Function to render a textured mesh with material properties
void renderTexturedMesh(const TexturedMesh& mesh, GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    
    // Set transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    // Set material properties
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), mesh.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(mesh.specular));
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);
    
    // Render mesh
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Function to create a cafe counter with different materials
void createCafeCounter(std::vector<TexturedMesh>& meshes, float width = 4.0f, float depth = 1.5f, float height = 1.0f) {
    // Counter top (light grey marble)
    TexturedMesh counterTop;
    
    // Counter top vertices
    std::vector<float> topVertices = {
        // Top face
        -width/2, height, -depth/2,    width/2, height, -depth/2,    width/2, height, depth/2,    -width/2, height, depth/2,
        // Front edge
        -width/2, height-0.05f, depth/2,    width/2, height-0.05f, depth/2,    width/2, height, depth/2,    -width/2, height, depth/2,
        // Back edge  
        -width/2, height, -depth/2,    width/2, height, -depth/2,    width/2, height-0.05f, -depth/2,    -width/2, height-0.05f, -depth/2,
        // Left edge
        -width/2, height, -depth/2,    -width/2, height, depth/2,    -width/2, height-0.05f, depth/2,    -width/2, height-0.05f, -depth/2,
        // Right edge
        width/2, height, -depth/2,    width/2, height-0.05f, -depth/2,    width/2, height-0.05f, depth/2,    width/2, height, depth/2
    };
    
    // Texture coordinates for counter top
    std::vector<float> topTexCoords = {
        // Top face
        0.0f, 0.0f,    2.0f, 0.0f,    2.0f, 1.0f,    0.0f, 1.0f,
        // Front edge
        0.0f, 0.0f,    2.0f, 0.0f,    2.0f, 0.1f,    0.0f, 0.1f,
        // Back edge
        0.0f, 0.0f,    2.0f, 0.0f,    2.0f, 0.1f,    0.0f, 0.1f,
        // Left edge
        0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 0.1f,    0.0f, 0.1f,
        // Right edge
        0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 0.1f,    0.0f, 0.1f
    };
    
    std::vector<unsigned int> topIndices = {
        // Top face
        0, 1, 2,    2, 3, 0,
        // Front edge
        4, 5, 6,    6, 7, 4,
        // Back edge
        8, 9, 10,   10, 11, 8,
        // Left edge
        12, 13, 14, 14, 15, 12,
        // Right edge
        16, 17, 18, 18, 19, 16
    };
    
    counterTop.vertices = topVertices;
    counterTop.texCoords = topTexCoords;
    counterTop.indices = topIndices;
    counterTop.textureID = generateLightGreyMarbleTexture();
    counterTop.shininess = 32.0f;
    counterTop.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    
    // Counter base (dark shiny grey)
    TexturedMesh counterBase;
    
    // Counter base vertices
    std::vector<float> baseVertices = {
        // Front face
        -width/2, 0.0f, depth/2,    width/2, 0.0f, depth/2,    width/2, height-0.05f, depth/2,    -width/2, height-0.05f, depth/2,
        // Back face
        -width/2, 0.0f, -depth/2,   -width/2, height-0.05f, -depth/2,   width/2, height-0.05f, -depth/2,   width/2, 0.0f, -depth/2,
        // Left face
        -width/2, 0.0f, -depth/2,   -width/2, 0.0f, depth/2,    -width/2, height-0.05f, depth/2,    -width/2, height-0.05f, -depth/2,
        // Right face
        width/2, 0.0f, -depth/2,    width/2, height-0.05f, -depth/2,    width/2, height-0.05f, depth/2,    width/2, 0.0f, depth/2,
        // Bottom face
        -width/2, 0.0f, -depth/2,   width/2, 0.0f, -depth/2,    width/2, 0.0f, depth/2,    -width/2, 0.0f, depth/2
    };
    
    std::vector<float> baseTexCoords = {
        // Front face
        0.0f, 0.0f,    2.0f, 0.0f,    2.0f, 1.0f,    0.0f, 1.0f,
        // Back face
        0.0f, 0.0f,    0.0f, 1.0f,    2.0f, 1.0f,    2.0f, 0.0f,
        // Left face
        0.0f, 0.0f,    1.0f, 0.0f,    1.0f, 1.0f,    0.0f, 1.0f,
        // Right face
        0.0f, 0.0f,    0.0f, 1.0f,    1.0f, 1.0f,    1.0f, 0.0f,
        // Bottom face
        0.0f, 0.0f,    2.0f, 0.0f,    2.0f, 1.0f,    0.0f, 1.0f
    };
    
    std::vector<unsigned int> baseIndices = {
        // Front face
        0, 1, 2,    2, 3, 0,
        // Back face
        4, 5, 6,    6, 7, 4,
        // Left face
        8, 9, 10,   10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Bottom face
        16, 17, 18, 18, 19, 16
    };
    
    counterBase.vertices = baseVertices;
    counterBase.texCoords = baseTexCoords;
    counterBase.indices = baseIndices;
    counterBase.textureID = generateDarkShinyGreyTexture();
    counterBase.shininess = 128.0f;  // Higher shininess for the shiny dark base
    counterBase.specular = glm::vec3(0.8f, 0.8f, 0.8f);  // High specular for shininess
    
    // Calculate normals for both meshes
    calculateNormals(counterTop);
    calculateNormals(counterBase);
    
    // Setup OpenGL buffers for both meshes
    setupMeshBuffers(counterTop);
    setupMeshBuffers(counterBase);
    
    meshes.push_back(counterTop);
    meshes.push_back(counterBase);
}

// Function to create multiple chocolate bars positioned on the counter
void createChocolateBarsOnCounter(std::vector<TexturedMesh>& chocolateBars, float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f) {
    // Clear existing chocolate bars
    chocolateBars.clear();
    
    // Create 10 chocolate bars
    for (int i = 0; i < 10; i++) {
        TexturedMesh chocolateBar;
        generateChocolateBar(chocolateBar);
        chocolateBars.push_back(chocolateBar);
    }
}

// Function to get transformation matrix for each chocolate bar on the counter
std::vector<glm::mat4> getChocolateBarTransforms(float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f) {
    std::vector<glm::mat4> transforms;
    
    // Height offset to place bars on top of counter (counter height + half of chocolate bar height)
    float yOffset = counterHeight + 0.025f; // 0.025f is half the chocolate bar height (0.05f / 2)
    
    // Arrange chocolate bars in two rows
    float rowSpacing = 0.4f;  // Distance between rows
    float colSpacing = 0.3f;  // Distance between columns
    
    // First row (5 bars) - closer to front
    float row1Z = counterDepth * 0.2f;  // 20% toward the front from center
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = glm::mat4(1.0f);
        
        // Position calculation: spread bars evenly across counter width
        float xPos = -counterWidth * 0.3f + (i * colSpacing);
        
        // Apply translation
        transform = glm::translate(transform, glm::vec3(xPos, yOffset, row1Z));
        
        // Add slight random rotation for more natural look
        float randomRotation = (i % 3 - 1) * 15.0f; // -15, 0, or 15 degrees
        transform = glm::rotate(transform, glm::radians(randomRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        
        transforms.push_back(transform);
    }
    
    // Second row (5 bars) - toward back
    float row2Z = -counterDepth * 0.2f;  // 20% toward the back from center
    for (int i = 0; i < 5; i++) {
        glm::mat4 transform = glm::mat4(1.0f);
        
        // Offset the second row slightly for a more natural arrangement
        float xPos = -counterWidth * 0.3f + (i * colSpacing) + 0.15f;
        
        // Apply translation
        transform = glm::translate(transform, glm::vec3(xPos, yOffset, row2Z));
        
        // Add slight random rotation
        float randomRotation = ((i + 2) % 3 - 1) * 12.0f; // Different rotation pattern
        transform = glm::rotate(transform, glm::radians(randomRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        
        transforms.push_back(transform);
    }
    
    return transforms;
}

// Modified rendering function to render all chocolate bars
// Modified rendering function to accept a base transformation matrix
void renderChocolateBarsOnCounter(const std::vector<TexturedMesh>& chocolateBars, GLuint shaderProgram, const glm::mat4& baseTransform, const glm::mat4& view, const glm::mat4& projection, float counterWidth = 4.0f, float counterDepth = 1.5f, float counterHeight = 1.0f) {
    if (chocolateBars.empty()) return;
    
    // Get all transformation matrices
    std::vector<glm::mat4> transforms = getChocolateBarTransforms(counterWidth, counterDepth, counterHeight);
    
    // Render each chocolate bar with its transformation combined with base transform
    for (size_t i = 0; i < chocolateBars.size() && i < transforms.size(); i++) {
        // Combine base transformation (rotation) with individual chocolate bar position
        glm::mat4 finalTransform = baseTransform * transforms[i];
        renderTexturedMesh(chocolateBars[i], shaderProgram, finalTransform, view, projection);
    }
}

//coffee machine
// Generate textures at startup
GLuint metalTexture;
GLuint redTexture; 
GLuint blackTexture;
GLuint glassTexture;

TexturedMesh createSimpleBox(float width, float height, float depth) {
    TexturedMesh box;
    
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;
    
    // Define vertices for a simple box
    box.vertices = {
        // Front face
        -w, -h,  d,  w, -h,  d,  w,  h,  d, -w,  h,  d,
        // Back face  
        -w, -h, -d, -w,  h, -d,  w,  h, -d,  w, -h, -d,
        // Top face
        -w,  h, -d, -w,  h,  d,  w,  h,  d,  w,  h, -d,
        // Bottom face
        -w, -h, -d,  w, -h, -d,  w, -h,  d, -w, -h,  d,
        // Right face
         w, -h, -d,  w,  h, -d,  w,  h,  d,  w, -h,  d,
        // Left face
        -w, -h, -d, -w, -h,  d, -w,  h,  d, -w,  h, -d
    };
    
    // Texture coordinates
    for (unsigned int i = 0; i < 6; i++) {
        box.texCoords.insert(box.texCoords.end(), {
            0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
        });
    }
    
    // Normals
    glm::vec3 normals[6] = {
        {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, 0}
    };
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            box.normals.insert(box.normals.end(), {normals[i].x, normals[i].y, normals[i].z});
        }
    }
    
    // Indices
    for (unsigned int i = 0; i < 6; i++) {
        unsigned int offset = i * 4;
        box.indices.insert(box.indices.end(), {
            offset, offset+1, offset+2, offset+2, offset+3, offset
        });
    }
    
    setupMeshBuffers(box);
    return box;
}

struct CoffeeMachine {
    TexturedMesh base;       // Main body (red)
    TexturedMesh top;        // Metal top part
    TexturedMesh waterTank;  // Transparent water tank
    TexturedMesh controlPanel;    // Control buttons
    TexturedMesh dripTray;   // Metal drip tray
};

TexturedMesh createCoffeeMachineBody() {
    TexturedMesh body;
    
    // Define dimensions for the coffee machine body
    float sizeX = 1.0f;  // Width (half)
    float sizeY = 1.5f;  // Height (half)
    float sizeZ = 0.5f;  // Depth (half)
    
    // Define all 8 vertices of the box
    // Front face vertices
    glm::vec3 frontBottomLeft(-sizeX, -sizeY, sizeZ);
    glm::vec3 frontBottomRight(sizeX, -sizeY, sizeZ);
    glm::vec3 frontTopRight(sizeX, sizeY, sizeZ);
    glm::vec3 frontTopLeft(-sizeX, sizeY, sizeZ);
    
    // Back face vertices (z inverted)
    glm::vec3 backBottomLeft(-sizeX, -sizeY, -sizeZ);
    glm::vec3 backBottomRight(sizeX, -sizeY, -sizeZ);
    glm::vec3 backTopRight(sizeX, sizeY, -sizeZ);
    glm::vec3 backTopLeft(-sizeX, sizeY, -sizeZ);
    
    // Define all 6 faces (24 vertices for GL_TRIANGLES)
    // Front face
    body.vertices = {
        // Front face
        frontBottomLeft.x, frontBottomLeft.y, frontBottomLeft.z,
        frontBottomRight.x, frontBottomRight.y, frontBottomRight.z,
        frontTopRight.x, frontTopRight.y, frontTopRight.z,
        frontTopLeft.x, frontTopLeft.y, frontTopLeft.z,
        
        // Back face
        backBottomRight.x, backBottomRight.y, backBottomRight.z,
        backBottomLeft.x, backBottomLeft.y, backBottomLeft.z,
        backTopLeft.x, backTopLeft.y, backTopLeft.z,
        backTopRight.x, backTopRight.y, backTopRight.z,
        
        // Top face
        frontTopLeft.x, frontTopLeft.y, frontTopLeft.z,
        frontTopRight.x, frontTopRight.y, frontTopRight.z,
        backTopRight.x, backTopRight.y, backTopRight.z,
        backTopLeft.x, backTopLeft.y, backTopLeft.z,
        
        // Bottom face
        frontBottomRight.x, frontBottomRight.y, frontBottomRight.z,
        frontBottomLeft.x, frontBottomLeft.y, frontBottomLeft.z,
        backBottomLeft.x, backBottomLeft.y, backBottomLeft.z,
        backBottomRight.x, backBottomRight.y, backBottomRight.z,
        
        // Left face
        backBottomLeft.x, backBottomLeft.y, backBottomLeft.z,
        frontBottomLeft.x, frontBottomLeft.y, frontBottomLeft.z,
        frontTopLeft.x, frontTopLeft.y, frontTopLeft.z,
        backTopLeft.x, backTopLeft.y, backTopLeft.z,
        
        // Right face
        frontBottomRight.x, frontBottomRight.y, frontBottomRight.z,
        backBottomRight.x, backBottomRight.y, backBottomRight.z,
        backTopRight.x, backTopRight.y, backTopRight.z,
        frontTopRight.x, frontTopRight.y, frontTopRight.z
    };
    
    // Texture coordinates (each face gets its own UV space)
    body.texCoords = {
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        
        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    // Normals for each face
    glm::vec3 frontNormal(0.0f, 0.0f, 1.0f);
    glm::vec3 backNormal(0.0f, 0.0f, -1.0f);
    glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
    glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
    glm::vec3 leftNormal(-1.0f, 0.0f, 0.0f);
    glm::vec3 rightNormal(1.0f, 0.0f, 0.0f);
    
    // Add normals for each vertex (same normal for all vertices of a face)
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {frontNormal.x, frontNormal.y, frontNormal.z});
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {backNormal.x, backNormal.y, backNormal.z});
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {topNormal.x, topNormal.y, topNormal.z});
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {bottomNormal.x, bottomNormal.y, bottomNormal.z});
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {leftNormal.x, leftNormal.y, leftNormal.z});
    for (int i = 0; i < 4; i++) body.normals.insert(body.normals.end(), {rightNormal.x, rightNormal.y, rightNormal.z});
    
    // Indices for all faces (6 faces × 2 triangles × 3 vertices = 36 indices)
    body.indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
        
        // Back face
        4, 5, 6,
        6, 7, 4,
        
        // Top face
        8, 9, 10,
        10, 11, 8,
        
        // Bottom face
        12, 13, 14,
        14, 15, 12,
        
        // Left face
        16, 17, 18,
        18, 19, 16,
        
        // Right face
        20, 21, 22,
        22, 23, 20
    };
    
    // Generate and bind VAO
    glGenVertexArrays(1, &body.VAO);
    glBindVertexArray(body.VAO);
    
    // Generate and bind VBO for vertices
    glGenBuffers(1, &body.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, body.VBO);
    glBufferData(GL_ARRAY_BUFFER, body.vertices.size() * sizeof(float), body.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Generate and bind VBO for normals
    glGenBuffers(1, &body.NBO);
    glBindBuffer(GL_ARRAY_BUFFER, body.NBO);
    glBufferData(GL_ARRAY_BUFFER, body.normals.size() * sizeof(float), body.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Generate and bind VBO for texture coordinates
    glGenBuffers(1, &body.TBO);
    glBindBuffer(GL_ARRAY_BUFFER, body.TBO);
    glBufferData(GL_ARRAY_BUFFER, body.texCoords.size() * sizeof(float), body.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    
    // Generate and bind EBO for indices
    glGenBuffers(1, &body.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, body.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, body.indices.size() * sizeof(unsigned int), body.indices.data(), GL_STATIC_DRAW);
    
    // Unbind VAO
    glBindVertexArray(0);
    
    // Assign the red texture
    body.textureID = redTexture;
    body.shininess = 32.0f;
    body.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    
    return body;
}

TexturedMesh createMetalTop() {
    TexturedMesh top = createSimpleBox(2.1f, 0.3f, 1.6f); // Slightly larger than body
    top.textureID = metalTexture;
    top.shininess = 128.0f; // More shiny for metal
    top.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    return top;
}

TexturedMesh createDripTray() {
    TexturedMesh tray;
    
    // Dimensions
    float width = 0.9f;
    float depth = 0.9f;
    float height = -0.25f;
    float rimHeight = 0.03f;
    
    // Vertices (simplified - in practice you'd want more detail)
    // Main tray
    glm::vec3 bl(-width/2, -height/2, -depth/2);
    glm::vec3 br(width/2, -height/2, -depth/2);
    glm::vec3 tr(width/2, -height/2, depth/2);
    glm::vec3 tl(-width/2, -height/2, depth/2);
    
    // Rim vertices
    glm::vec3 rimBl = bl + glm::vec3(0, rimHeight, 0);
    glm::vec3 rimBr = br + glm::vec3(0, rimHeight, 0);
    glm::vec3 rimTr = tr + glm::vec3(0, rimHeight, 0);
    glm::vec3 rimTl = tl + glm::vec3(0, rimHeight, 0);
    
    // Define vertices for bottom and rim
    tray.vertices = {
        // Bottom
        bl.x, bl.y, bl.z,
        br.x, br.y, br.z,
        tr.x, tr.y, tr.z,
        tl.x, tl.y, tl.z,
        
        // Rim
        rimBl.x, rimBl.y, rimBl.z,
        rimBr.x, rimBr.y, rimBr.z,
        rimTr.x, rimTr.y, rimTr.z,
        rimTl.x, rimTl.y, rimTl.z,
    };
    
    // Texture coordinates
    tray.texCoords = {
        // Bottom
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // Rim
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };
    
    // Normals
    for (int i = 0; i < 4; i++) {
        tray.normals.insert(tray.normals.end(), {0.0f, -1.0f, 0.0f}); // Bottom
    }
    for (int i = 0; i < 4; i++) {
        tray.normals.insert(tray.normals.end(), {0.0f, 1.0f, 0.0f}); // Rim top
    }
    
    // Indices
    tray.indices = {
        // Bottom
        0, 1, 2, 2, 3, 0,
        // Rim top
        4, 5, 6, 6, 7, 4,
        // Sides
        0, 4, 1, 1, 4, 5,
        1, 5, 2, 2, 5, 6,
        2, 6, 3, 3, 6, 7,
        3, 7, 0, 0, 7, 4
    };
    
    setupMeshBuffers(tray);
    
    tray.textureID = metalTexture;
    tray.shininess = 128.0f;
    tray.specular = glm::vec3(0.7f, 0.7f, 0.7f);
    
    return tray;
}

void addBoxSides(TexturedMesh& mesh, int baseIndex, glm::vec3 frontBL, glm::vec3 frontBR, glm::vec3 frontTR, glm::vec3 frontTL,glm::vec3 backBL, glm::vec3 backBR, glm::vec3 backTR, glm::vec3 backTL) {
    // Bottom face
    mesh.vertices.insert(mesh.vertices.end(), {
        frontBL.x, frontBL.y, frontBL.z,
        frontBR.x, frontBR.y, frontBR.z,
        backBR.x, backBR.y, backBR.z,
        backBL.x, backBL.y, backBL.z
    });
    
    // Top face
    mesh.vertices.insert(mesh.vertices.end(), {
        frontTL.x, frontTL.y, frontTL.z,
        frontTR.x, frontTR.y, frontTR.z,
        backTR.x, backTR.y, backTR.z,
        backTL.x, backTL.y, backTL.z
    });
    
    // Left face
    mesh.vertices.insert(mesh.vertices.end(), {
        frontBL.x, frontBL.y, frontBL.z,
        backBL.x, backBL.y, backBL.z,
        backTL.x, backTL.y, backTL.z,
        frontTL.x, frontTL.y, frontTL.z
    });
    
    // Right face
    mesh.vertices.insert(mesh.vertices.end(), {
        frontBR.x, frontBR.y, frontBR.z,
        backBR.x, backBR.y, backBR.z,
        backTR.x, backTR.y, backTR.z,
        frontTR.x, frontTR.y, frontTR.z
    });
    
    // Add normals for sides
    for (int i = 0; i < 4; i++) mesh.normals.insert(mesh.normals.end(), {0.0f, -1.0f, 0.0f}); // Bottom
    for (int i = 0; i < 4; i++) mesh.normals.insert(mesh.normals.end(), {0.0f, 1.0f, 0.0f});  // Top
    for (int i = 0; i < 4; i++) mesh.normals.insert(mesh.normals.end(), {-1.0f, 0.0f, 0.0f}); // Left
    for (int i = 0; i < 4; i++) mesh.normals.insert(mesh.normals.end(), {1.0f, 0.0f, 0.0f});  // Right
    
    // Add texture coordinates for sides
    for (int face = 0; face < 4; face++) {
        mesh.texCoords.insert(mesh.texCoords.end(), {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        });
    }
    
    // Add indices for sides
    for (unsigned int face = 0; face < 4; face++) {
        unsigned int offset = baseIndex + face * 4;
        mesh.indices.insert(mesh.indices.end(), {
            offset, offset+1, offset+2,
            offset+2, offset+3, offset
        });
    }
}

TexturedMesh createWaterTank() {
    TexturedMesh tank;
    
    // Dimensions
    const float width = 0.6f;
    const float height = 0.8f;
    const float depth = 0.3f;
    const float thickness = 0.02f;
    
    // Outer vertices
    glm::vec3 frontBottomLeftOuter(-width/2, -height/2, depth/2);
    glm::vec3 frontBottomRightOuter(width/2, -height/2, depth/2);
    glm::vec3 frontTopRightOuter(width/2, height/2, depth/2);
    glm::vec3 frontTopLeftOuter(-width/2, height/2, depth/2);
    
    glm::vec3 backBottomLeftOuter(-width/2, -height/2, -depth/2);
    glm::vec3 backBottomRightOuter(width/2, -height/2, -depth/2);
    glm::vec3 backTopRightOuter(width/2, height/2, -depth/2);
    glm::vec3 backTopLeftOuter(-width/2, height/2, -depth/2);
    
    // Inner vertices (offset by thickness)
    glm::vec3 frontBottomLeftInner = frontBottomLeftOuter + glm::vec3(thickness, thickness, -thickness);
    glm::vec3 frontBottomRightInner = frontBottomRightOuter + glm::vec3(-thickness, thickness, -thickness);
    glm::vec3 frontTopRightInner = frontTopRightOuter + glm::vec3(-thickness, -thickness, -thickness);
    glm::vec3 frontTopLeftInner = frontTopLeftOuter + glm::vec3(thickness, -thickness, -thickness);
    
    glm::vec3 backBottomLeftInner = backBottomLeftOuter + glm::vec3(thickness, thickness, thickness);
    glm::vec3 backBottomRightInner = backBottomRightOuter + glm::vec3(-thickness, thickness, thickness);
    glm::vec3 backTopRightInner = backTopRightOuter + glm::vec3(-thickness, -thickness, thickness);
    glm::vec3 backTopLeftInner = backTopLeftOuter + glm::vec3(thickness, -thickness, thickness);
    
    // Simplified tank - just outer faces for now
    tank.vertices = {
        // Front face
        frontBottomLeftOuter.x, frontBottomLeftOuter.y, frontBottomLeftOuter.z,
        frontBottomRightOuter.x, frontBottomRightOuter.y, frontBottomRightOuter.z,
        frontTopRightOuter.x, frontTopRightOuter.y, frontTopRightOuter.z,
        frontTopLeftOuter.x, frontTopLeftOuter.y, frontTopLeftOuter.z,
        
        // Back face
        backBottomLeftOuter.x, backBottomLeftOuter.y, backBottomLeftOuter.z,
        backBottomRightOuter.x, backBottomRightOuter.y, backBottomRightOuter.z,
        backTopRightOuter.x, backTopRightOuter.y, backTopRightOuter.z,
        backTopLeftOuter.x, backTopLeftOuter.y, backTopLeftOuter.z,
    };
    
    // Add remaining faces using addBoxSides
    addBoxSides(tank, 8, frontBottomLeftOuter, frontBottomRightOuter, frontTopRightOuter, frontTopLeftOuter,
               backBottomLeftOuter, backBottomRightOuter, backTopRightOuter, backTopLeftOuter);
    
    // Texture coordinates for front and back faces
    tank.texCoords = {
        // Front face
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // Back face  
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };
    
    // Normals for front and back faces
    for (int i = 0; i < 4; i++) tank.normals.insert(tank.normals.end(), {0.0f, 0.0f, 1.0f});   // Front
    for (int i = 0; i < 4; i++) tank.normals.insert(tank.normals.end(), {0.0f, 0.0f, -1.0f});  // Back
    
    // Indices for front and back faces
    tank.indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 7, 6, 6, 5, 4,
    };
    
    setupMeshBuffers(tank);
    
    tank.textureID = glassTexture;
    tank.shininess = 256.0f;
    tank.specular = glm::vec3(0.9f, 0.9f, 0.9f);
    
    return tank;
}

TexturedMesh createControlPanel() {
    TexturedMesh panel;
    
    // Dimensions
    const float width = 0.8f;
    const float height = 0.3f;
    const float depth = 0.05f;
    const float buttonRadius = 0.04f;
    const int buttonRows = 2;
    const int buttonCols = 3;
    const float displayWidth = 0.3f;
    const float displayHeight = 0.15f;
    
    // Main panel vertices
    glm::vec3 frontBL(-width/2, -height/2, 0.0f);
    glm::vec3 frontBR(width/2, -height/2, 0.0f);
    glm::vec3 frontTR(width/2, height/2, 0.0f);
    glm::vec3 frontTL(-width/2, height/2, 0.0f);
    glm::vec3 backBL(-width/2, -height/2, -depth);
    glm::vec3 backBR(width/2, -height/2, -depth);
    glm::vec3 backTR(width/2, height/2, -depth);
    glm::vec3 backTL(-width/2, height/2, -depth);
    
    // Main panel vertices
    panel.vertices = {
        // Front face
        frontBL.x, frontBL.y, frontBL.z,
        frontBR.x, frontBR.y, frontBL.z,
        frontTR.x, frontTR.y, frontTR.z,
        frontTL.x, frontTL.y, frontTL.z,
        
        // Back face
        backBL.x, backBL.y, backBL.z,
        backBR.x, backBR.y, backBR.z,
        backTR.x, backTR.y, backTR.z,
        backTL.x, backTL.y, backTL.z,
    };
    
    // Add side faces
    addBoxSides(panel, 8, frontBL, frontBR, frontTR, frontTL, backBL, backBR, backTR, backTL);
    
    // Texture coordinates
    panel.texCoords = {
        // Front face
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        // Back face
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };
    
    // Normals
    for (int i = 0; i < 4; i++) panel.normals.insert(panel.normals.end(), {0.0f, 0.0f, 1.0f});  // Front
    for (int i = 0; i < 4; i++) panel.normals.insert(panel.normals.end(), {0.0f, 0.0f, -1.0f}); // Back
    
    // Indices
    panel.indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
    };
    
    setupMeshBuffers(panel);
    
    panel.textureID = blackTexture;
    panel.shininess = 64.0f;
    panel.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    
    return panel;
}

CoffeeMachine createCoffeeMachine() {
    CoffeeMachine machine;
    
    // Create machine components with corrected function calls
    std::cout << "Creating base...\n";
    machine.base = createCoffeeMachineBody();
    std::cout << "Creating top...\n";
    machine.top = createMetalTop();
    std::cout << "Creating water tank...\n";
    machine.waterTank = createWaterTank();
    std::cout << "Creating control panel...\n";
    machine.controlPanel = createControlPanel();
    std::cout << "Creating drip tray...\n";
    machine.dripTray = createDripTray();
    
    return machine;
}

void renderCoffeeMachine(const CoffeeMachine& machine, GLuint shaderProgram, const glm::mat4& modelMatrix) {
    glUseProgram(shaderProgram);
    
    glm::mat4 adjustedModel = glm::translate(modelMatrix, glm::vec3(1.8f, 4.1f, 0.0f));

    // Render base (main body) - no transformation needed
    glm::mat4 baseModel = adjustedModel;
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(baseModel));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), machine.base.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(machine.base.specular));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, machine.base.textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
    glBindVertexArray(machine.base.VAO);
    glDrawElements(GL_TRIANGLES, machine.base.indices.size(), GL_UNSIGNED_INT, 0);
    
    // Render top (positioned above base)
    glm::mat4 topModel = glm::translate(adjustedModel, glm::vec3(0.0f, 1.65f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(topModel));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), machine.top.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(machine.top.specular));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, machine.top.textureID);
    glBindVertexArray(machine.top.VAO);
    glDrawElements(GL_TRIANGLES, machine.top.indices.size(), GL_UNSIGNED_INT, 0);
    
    // Render water tank (positioned at back-left)
    glm::mat4 tankModel = glm::translate(adjustedModel, glm::vec3(-0.7f, 0.3f, -0.3f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(tankModel));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), machine.waterTank.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(machine.waterTank.specular));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, machine.waterTank.textureID);
    glBindVertexArray(machine.waterTank.VAO);
    glDrawElements(GL_TRIANGLES, machine.waterTank.indices.size(), GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    // Render control panel (positioned on front face)
    glm::mat4 panelModel = glm::translate(adjustedModel, glm::vec3(0.0f, 0.2f, 0.55f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(panelModel));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), machine.controlPanel.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(machine.controlPanel.specular));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, machine.controlPanel.textureID);
    glBindVertexArray(machine.controlPanel.VAO);
    glDrawElements(GL_TRIANGLES, machine.controlPanel.indices.size(), GL_UNSIGNED_INT, 0);
    
    // Render drip tray (positioned at bottom front)
    glm::mat4 trayModel = glm::translate(adjustedModel, glm::vec3(0.0f, -1.6f, 0.3f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(trayModel));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), machine.dripTray.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(machine.dripTray.specular));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, machine.dripTray.textureID);
    glBindVertexArray(machine.dripTray.VAO);
    glDrawElements(GL_TRIANGLES, machine.dripTray.indices.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

void cleanupCoffeeMachine(const CoffeeMachine& machine) {
    // Clean up base
    glDeleteVertexArrays(1, &machine.base.VAO);
    glDeleteBuffers(1, &machine.base.VBO);
    glDeleteBuffers(1, &machine.base.NBO);
    glDeleteBuffers(1, &machine.base.TBO);
    glDeleteBuffers(1, &machine.base.EBO);
    
    // Clean up other components
    glDeleteVertexArrays(1, &machine.top.VAO);
    glDeleteBuffers(1, &machine.top.VBO);
    glDeleteBuffers(1, &machine.top.EBO);
    
    glDeleteVertexArrays(1, &machine.waterTank.VAO);
    glDeleteBuffers(1, &machine.waterTank.VBO);
    glDeleteBuffers(1, &machine.waterTank.EBO);
    
    glDeleteVertexArrays(1, &machine.controlPanel.VAO);
    glDeleteBuffers(1, &machine.controlPanel.VBO);
    glDeleteBuffers(1, &machine.controlPanel.EBO);
    
    glDeleteVertexArrays(1, &machine.dripTray.VAO);
    glDeleteBuffers(1, &machine.dripTray.VBO);
    glDeleteBuffers(1, &machine.dripTray.EBO);
}








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
    GLuint ObjectShaders = LoadShaders("object_vertex.glsl", "object_fragment.glsl");

    Sun sun;
    Roof roof(4.0f, 33.0f, 30, 10);
    Ribs ribs(4.0f, 33.0f, 8, 30);
    Moon moon;
    setupScene();

    //Plants

// Create 4 plants at floor corners
std::vector<PlantMesh> plants(4);
std::vector<glm::vec3> plantPositions = {
    glm::vec3(-3.2f, 0.2f,  0.7f),   // Front left corner
    glm::vec3( 3.2f, 0.2f,  0.7f),   // Front right corner  
    glm::vec3( 3.2f, 0.2f, -28.7f),  // Back right corner
    glm::vec3(-3.2f, 0.2f, -28.7f)   // Back left corner
};

// Generate leaf configurations for each plant using your function
std::vector<std::vector<Leaf>> plantLeaves(4);
    for (int p = 0; p < 4; p++) {
        int leafCount = 20 + (p * 2); // Different leaf counts: 8, 10, 12, 14 leaves per plant
        plantLeaves[p] = generateRandomLeaves(leafCount);
    }

    // Generate all plant meshes
    for (int i = 0; i < 4; i++) {
        generatePlant(plants[i], plantLeaves[i]);
    }



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

        //OBJECTS
        glUseProgram(ObjectShaders);
        glUniformMatrix4fv(glGetUniformLocation(ObjectShaders, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ObjectShaders, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(ObjectShaders, "lightPos"), 3.0f, 10.0f, 3.0f);
        glUniform3f(glGetUniformLocation(ObjectShaders, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(ObjectShaders, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

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

        // Render plants
        renderPlants(ObjectShaders, plants , plantPositions, view, projection);
        
        // Draw Roof (Transparent Yellow Semi-Cylinder)
        glUseProgram(roofShader);
        glm::mat4 roofModel = glm::mat4(1.0f);
        roofModel = glm::translate(roofModel, glm::vec3(0.0f, 9.0f, -31.0f)); // Start at north wall (Z=1.5)
        roofModel = glm::rotate(roofModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // scale to perfectly match wall dimensions
        roofModel = glm::scale(roofModel, glm::vec3(1.0f, 1.0f, -1.0f)); // Ensure proper facing

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

        //Add OBJECTS
        // Model matrix for carpet (on the floor)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
