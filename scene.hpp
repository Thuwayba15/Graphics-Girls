#ifndef SCENE_HPP
#define SCENE_HPP

void setupScene();
void drawScene(GLuint shader);
void drawBox(GLuint shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color);

#endif
