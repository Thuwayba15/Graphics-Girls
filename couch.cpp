#include "couch.hpp"
#include "texture_loader.hpp" // for generateWoodTexture, generateFabricTexture
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <GL/glew.h>
#include <glm/glm.hpp>

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