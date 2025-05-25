#include "plant.hpp"
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

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

// struct Leaf {
//     float angle;
//     float height;
//     float size;
//     float bend;
// };

const int NUM_LEAVES = 30; // Number of leaves to generate


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

void renderPlants(GLuint shader, const std::vector<PlantMesh>& plants, const std::vector<glm::vec3>& positions) {
    for (size_t i = 0; i < plants.size(); ++i) {
        const PlantMesh& plant = plants[i];

        glm::mat4 model = glm::translate(glm::mat4(1.0f), positions[i]);
        model = glm::scale(model, glm::vec3(0.35f)); // Adjust to realistic scale
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(plant.VAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shader, "texture1"), 0);

        // Pot
        glBindTexture(GL_TEXTURE_2D, plant.potTexture);
        glUniform1f(glGetUniformLocation(shader, "shininess"), plant.potShininess);
        glUniform3fv(glGetUniformLocation(shader, "specular"), 1, glm::value_ptr(plant.potSpecular));
        glDrawElements(GL_TRIANGLES, plant.potIndexCount, GL_UNSIGNED_INT, 0);

        // Soil (immediately after pot)
        glBindTexture(GL_TEXTURE_2D, plant.soilTexture);
        glUniform1f(glGetUniformLocation(shader, "shininess"), plant.soilShininess);
        glUniform3fv(glGetUniformLocation(shader, "specular"), 1, glm::value_ptr(plant.soilSpecular));
        int soilOffset = plant.potIndexCount;
        int soilCount = plant.stemIndexCount > 0 ? (plant.stemIndexCount) : 0;
        glDrawElements(GL_TRIANGLES, soilCount, GL_UNSIGNED_INT, (void*)(soilOffset * sizeof(unsigned int)));

        // Stem (after pot + soil)
        glBindTexture(GL_TEXTURE_2D, plant.stemTexture);
        glUniform1f(glGetUniformLocation(shader, "shininess"), plant.stemShininess);
        glUniform3fv(glGetUniformLocation(shader, "specular"), 1, glm::value_ptr(plant.stemSpecular));
        glDrawElements(GL_TRIANGLES, plant.stemIndexCount, GL_UNSIGNED_INT, (void*)((plant.potIndexCount + soilCount) * sizeof(unsigned int)));

        // Leaves
        glBindTexture(GL_TEXTURE_2D, plant.leafTexture);
        glUniform1f(glGetUniformLocation(shader, "shininess"), plant.leafShininess);
        glUniform3fv(glGetUniformLocation(shader, "specular"), 1, glm::value_ptr(plant.leafSpecular));
        glDrawElements(GL_TRIANGLES, plant.leafIndexCount, GL_UNSIGNED_INT, (void*)((plant.potIndexCount + plant.stemIndexCount + soilCount) * sizeof(unsigned int)));

        glBindVertexArray(0);
    }
}
