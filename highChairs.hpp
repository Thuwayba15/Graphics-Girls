// highChairs.hpp
#ifndef HIGHCHAIRS_HPP
#define HIGHCHAIRS_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class HighChair
{
private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateChairGeometry();
    void setupBuffers();

public:
    HighChair();
    ~HighChair();

    void initialize(GLuint shader);
    void render(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);
    void cleanup();
};

#endif
