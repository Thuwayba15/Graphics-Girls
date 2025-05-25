#include "coffeeMachine.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

//coffee machine
GLuint metalTexture;
GLuint redTexture;
GLuint blackTexture;
GLuint glassTexture;
// GLuint generateMetalTexture() {
//     const int width = 128;
//     const int height = 128;
//     std::vector<unsigned char> data(width * height * 3);
    
//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             int index = (y * width + x) * 3;
            
//             // Base metal color (brushed steel)
//             float baseR = 0.7f;
//             float baseG = 0.7f;
//             float baseB = 0.75f;
            
//             // Create brushed metal effect
//             float brush = sin(y * 0.5f) * 0.1f;
//             float scratch = sin(x * 0.1f + y * 0.05f) * 0.05f;
            
//             // Add reflective variation
//             float reflection = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
            
//             data[index] = (unsigned char)((baseR + brush + scratch + reflection) * 255);
//             data[index + 1] = (unsigned char)((baseG + brush + scratch + reflection) * 255);
//             data[index + 2] = (unsigned char)((baseB + brush + scratch + reflection) * 255);
//         }
//     }
    
//     GLuint textureID;
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_2D, textureID);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
//     glGenerateMipmap(GL_TEXTURE_2D);
    
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//     return textureID;
// }

// GLuint generateGlassTexture() {
//     const int width = 512;
//     const int height = 512;
//     std::vector<unsigned char> data(width * height * 4); // RGBA for transparency
    
//     for (int y = 0; y < height; y++) {
//         for (int x = 0; x < width; x++) {
//             int index = (y * width + x) * 4;
            
//             // Base glass color (slightly tinted)
//             data[index] = 240;     // R
//             data[index + 1] = 245; // G
//             data[index + 2] = 250; // B
            
//             // Create floral pattern
//             float flowerAlpha = 0.0f;
            
//             // Multiple flower centers
//             for (int f = 0; f < 8; f++) {
//                 float fx = width * (0.2f + (f % 3) * 0.3f);
//                 float fy = height * (0.2f + (f / 3) * 0.3f);
//                 float dist = sqrt(pow(x - fx, 2) + pow(y - fy, 2));
                
//                 // Flower petals
//                 if (dist < 40.0f && dist > 10.0f) {
//                     float angle = atan2(y - fy, x - fx);
//                     float petal = sin(angle * 3.0f) * 0.5f + 0.5f; // 6 petals
//                     if (petal > 0.3f) {
//                         flowerAlpha = std::max(flowerAlpha, 0.6f * (1.0f - dist / 40.0f));
//                     }
//                 }
                
//                 // Flower center
//                 if (dist < 12.0f) {
//                     flowerAlpha = std::max(flowerAlpha, 0.8f);
//                 }
//             }
            
//             data[index + 3] = (unsigned char)(flowerAlpha * 255); // Alpha
//         }
//     }
    
//     GLuint textureID;
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_2D, textureID);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
//     glGenerateMipmap(GL_TEXTURE_2D);
    
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//     return textureID;
// }


// Function to setup OpenGL buffers for a textured mesh
// void setupMeshBuffers(TexturedMesh& mesh) {
//     // Generate and bind VAO
//     glGenVertexArrays(1, &mesh.VAO);
//     glBindVertexArray(mesh.VAO);
    
//     // Setup vertex buffer (VBO)
//     glGenBuffers(1, &mesh.VBO);
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
    
//     // Setup normal buffer (NBO)
//     glGenBuffers(1, &mesh.NBO);
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.NBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(1);
    
//     // Setup texture coordinate buffer (TBO)
//     glGenBuffers(1, &mesh.TBO);
//     glBindBuffer(GL_ARRAY_BUFFER, mesh.TBO);
//     glBufferData(GL_ARRAY_BUFFER, mesh.texCoords.size() * sizeof(float), mesh.texCoords.data(), GL_STATIC_DRAW);
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(2);
    
//     // Setup element buffer (EBO)
//     glGenBuffers(1, &mesh.EBO);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
//     // Unbind VAO
//     glBindVertexArray(0);
// }


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

// struct CoffeeMachine {
//     TexturedMesh base;       // Main body (red)
//     TexturedMesh top;        // Metal top part
//     TexturedMesh waterTank;  // Transparent water tank
//     TexturedMesh controlPanel;    // Control buttons
//     TexturedMesh dripTray;   // Metal drip tray
// };

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

CoffeeMachine buildCoffeeMachine() {
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
    // glDisable(GL_BLEND);
    
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