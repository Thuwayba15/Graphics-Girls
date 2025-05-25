#ifndef COUCH_HPP
#define COUCH_HPP

#include "textured_mesh.hpp"

void generateCouch(TexturedMesh& mesh, float x, float y, float z, float width, float height, float depth, int segments);
void generateBox(TexturedMesh& mesh, float x, float y, float z, float width, float height, float depth);
void generateCushion(TexturedMesh& mesh, float x, float y, float z, float width, float height, float depth, int segments);
void generateArmrest(TexturedMesh& mesh, float x, float y, float z, float width, float height, float depth, int segments);
void generateBackrest(TexturedMesh& mesh, float x, float y, float z, float width, float height, float depth, int segments);

#endif // COUCH_HPP
