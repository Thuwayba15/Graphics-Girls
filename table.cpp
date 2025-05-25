#include "table.hpp"
#include "texture_loader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

// void generateChocolateBar(TexturedMesh &mesh) {
//     // Clear any existing data
//     mesh.vertices.clear();
//     mesh.normals.clear();
//     mesh.texCoords.clear();
//     mesh.indices.clear();

//     float width = 0.15f;
//     float height = 0.05f;
//     float depth = 0.08f;
    
//     // Simple rectangular prism
//     float halfWidth = width / 2.0f;
//     float halfHeight = height / 2.0f;
//     float halfDepth = depth / 2.0f;

//     // Vertices
//     std::vector<float> positions = {
//         // Front face
//         -halfWidth, -halfHeight, halfDepth,
//          halfWidth, -halfHeight, halfDepth,
//          halfWidth,  halfHeight, halfDepth,
//         -halfWidth,  halfHeight, halfDepth,
        
//         // Back face
//         -halfWidth, -halfHeight, -halfDepth,
//          halfWidth, -halfHeight, -halfDepth,
//          halfWidth,  halfHeight, -halfDepth,
//         -halfWidth,  halfHeight, -halfDepth,
        
//         // Left face
//         -halfWidth, -halfHeight, -halfDepth,
//         -halfWidth,  halfHeight, -halfDepth,
//         -halfWidth,  halfHeight, halfDepth,
//         -halfWidth, -halfHeight, halfDepth,
        
//         // Right face
//         halfWidth, -halfHeight, -halfDepth,
//         halfWidth,  halfHeight, -halfDepth,
//         halfWidth,  halfHeight, halfDepth,
//         halfWidth, -halfHeight, halfDepth,
        
//         // Top face
//         -halfWidth, halfHeight, -halfDepth,
//          halfWidth, halfHeight, -halfDepth,
//          halfWidth, halfHeight, halfDepth,
//         -halfWidth, halfHeight, halfDepth,
        
//         // Bottom face
//         -halfWidth, -halfHeight, -halfDepth,
//          halfWidth, -halfHeight, -halfDepth,
//          halfWidth, -halfHeight, halfDepth,
//         -halfWidth, -halfHeight, halfDepth
//     };

//     // Normals
//     std::vector<float> normals = {
//         // Front face
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f, 1.0f,
        
//         // Back face
//         0.0f, 0.0f, -1.0f,
//         0.0f, 0.0f, -1.0f,
//         0.0f, 0.0f, -1.0f,
//         0.0f, 0.0f, -1.0f,
        
//         // Left face
//         -1.0f, 0.0f, 0.0f,
//         -1.0f, 0.0f, 0.0f,
//         -1.0f, 0.0f, 0.0f,
//         -1.0f, 0.0f, 0.0f,
        
//         // Right face
//         1.0f, 0.0f, 0.0f,
//         1.0f, 0.0f, 0.0f,
//         1.0f, 0.0f, 0.0f,
//         1.0f, 0.0f, 0.0f,
        
//         // Top face
//         0.0f, 1.0f, 0.0f,
//         0.0f, 1.0f, 0.0f,
//         0.0f, 1.0f, 0.0f,
//         0.0f, 1.0f, 0.0f,
        
//         // Bottom face
//         0.0f, -1.0f, 0.0f,
//         0.0f, -1.0f, 0.0f,
//         0.0f, -1.0f, 0.0f,
//         0.0f, -1.0f, 0.0f
//     };

//     // Texture coordinates
//     std::vector<float> texCoords = {
//         // Front face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f,
        
//         // Back face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f,
        
//         // Left face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f,
        
//         // Right face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f,
        
//         // Top face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f,
        
//         // Bottom face
//         0.0f, 0.0f,
//         1.0f, 0.0f,
//         1.0f, 1.0f,
//         0.0f, 1.0f
//     };

//     // Indices
//     std::vector<unsigned int> indices = {
//         // Front face
//         0, 1, 2,
//         2, 3, 0,
        
//         // Back face
//         4, 5, 6,
//         6, 7, 4,
        
//         // Left face
//         8, 9, 10,
//         10, 11, 8,
        
//         // Right face
//         12, 13, 14,
//         14, 15, 12,
        
//         // Top face
//         16, 17, 18,
//         18, 19, 16,
        
//         // Bottom face
//         20, 21, 22,
//         22, 23, 20
//     };

//     // Assign to mesh
//     mesh.vertices = positions;
//     mesh.normals = normals;
//     mesh.texCoords = texCoords;
//     mesh.indices = indices;

//     // Generate brown texture
//     mesh.textureID = generateSolidColorTexture(0.4f, 0.2f, 0.1f);
//     mesh.shininess = 16.0f;
//     mesh.specular = glm::vec3(0.1f, 0.1f, 0.1f);

//     // Create OpenGL buffers
//     glGenVertexArrays(1, &mesh.VAO);
//     glGenBuffers(1, &mesh.VBO);
//     glGenBuffers(1, &mesh.EBO);
//     glGenBuffers(1, &mesh.NBO);
//     glGenBuffers(1, &mesh.TBO);
    
//     glBindVertexArray(mesh.VAO);
    
//     // Vertex buffer
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
    
//     // Normal buffer
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(1);
    
//     // Texture coordinates buffer
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(2);
    
//     // Element buffer
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
//     glBindVertexArray(0);
// }

void generateCounter(Counter &counter, float width, float depth, float height) {
    // Generate counter top
    generateCounterTop(counter.top, width, depth, 0.1f);
    
    // Generate legs (position them at the corners)
    float legOffsetX = width / 2.0f - 0.15f;
    float legOffsetZ = depth / 2.0f - 0.15f;
    float legHeight = height-0.1f; // Subtract counter top thickness
    
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

void renderCounter(const Counter& counter, GLuint shaderProgram, const glm::mat4& baseModel, float height) {
    glm::mat4 topModel = glm::translate(baseModel, glm::vec3(0.0f, height, 0.0f));
    renderMesh(counter.top, topModel, shaderProgram);

    float legHeight = height - 0.1f;
    float legOffsetX = 0.6f / 2.0f - 0.15f;
    float legOffsetZ = 0.6f / 2.0f - 0.15f;

    glm::vec3 legPositions[] = {
        {-legOffsetX, legHeight / 2.0f, -legOffsetZ}, // front left
        { legOffsetX, legHeight / 2.0f, -legOffsetZ}, // front right
        {-legOffsetX, legHeight / 2.0f,  legOffsetZ}, // back left
        { legOffsetX, legHeight / 2.0f,  legOffsetZ}, // back right
    };

    for (int i = 0; i < 4; ++i) {
        glm::mat4 legModel = glm::translate(baseModel, legPositions[i]);
        renderMesh(counter.legs[i], legModel, shaderProgram);
    }

    // Items
    glm::mat4 cupModel = glm::translate(baseModel, glm::vec3(-0.2f, height + 0.1f, 0.0f));
    cupModel = glm::scale(cupModel, glm::vec3(0.6f)); 
    renderMesh(counter.items[0], cupModel, shaderProgram);

    glm::mat4 barModel = glm::translate(baseModel, glm::vec3(0.2f, height + 0.01f, 0.0f));
    renderMesh(counter.items[1], barModel, shaderProgram);
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