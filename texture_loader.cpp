#define STB_IMAGE_IMPLEMENTATION
#include "texture_loader.hpp"
#include "stb_image.h"
#include <vector>
#include <iostream>

GLuint loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Wrapping and filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

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