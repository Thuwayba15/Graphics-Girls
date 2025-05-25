#include "cafeCounter.hpp"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

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


// Function to create a cafe counter with different materials
void createCafeCounter(std::vector<TexturedMesh>& meshes, float width, float depth, float height) {
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
void createChocolateBarsOnCounter(std::vector<TexturedMesh>& chocolateBars, float counterWidth, float counterDepth, float counterHeight) {
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
std::vector<glm::mat4> getChocolateBarTransforms(float counterWidth, float counterDepth, float counterHeight) {
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
void renderChocolateBarsOnCounter(const std::vector<TexturedMesh>& chocolateBars, GLuint shaderProgram, const glm::mat4& baseTransform, const glm::mat4& view, const glm::mat4& projection, float counterWidth, float counterDepth, float counterHeight) {
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

// Function to setup OpenGL buffers for a textured mesh
void setupMeshBuffers(TexturedMesh& mesh) {
    glGenVertexArrays(1, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    // === POSITION ===
    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // aPos at location 0
    glEnableVertexAttribArray(0);

    // === TEXCOORD ===
    glGenBuffers(1, &mesh.TBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // aTexCoord at location 1
    glEnableVertexAttribArray(1);

    // === NORMAL ===
    glGenBuffers(1, &mesh.NBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // aNormal at location 2
    glEnableVertexAttribArray(2);

    // === INDEX BUFFER ===
    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0); // Clean unbind
}


// Function to render a textured mesh with material properties
void renderTexturedMesh(const TexturedMesh& mesh, GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    // Set transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Set material properties
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), 1.0f); // Fully opaque
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), mesh.shininess);
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(mesh.specular));

    // Enable texture use and bind it
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.textureID);

    // Render
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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