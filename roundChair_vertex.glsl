#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 localPos = aPos; // calc. normal based on position for cylinder
    
    // for cylinder, normal calculation
    if (abs(localPos.y) < 0.01) {
        Normal = vec3(0.0, -1.0, 0.0);     // bottom face
    } else if (abs(localPos.y - 0.3) < 0.01 || abs(localPos.y - 0.6) < 0.01) {
        Normal = vec3(0.0, 1.0, 0.0); // top faces
    } else {
        Normal = normalize(vec3(localPos.x, 0.0, localPos.z));  // side faces
    }
    
    Normal = mat3(transpose(inverse(model))) * Normal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
