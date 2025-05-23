#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 objectColor;

void main() {
    vec3 base = texture(texture1, TexCoord).rgb;
    FragColor = vec4(base * objectColor, 1.0); // mix with color if needed
}
