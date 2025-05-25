#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // ambient lighting - stronger for day, weaker for night
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse lighting for both front & back faces
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    
    // calc diffuse for front-facing surfaces
    float diffFront = max(dot(norm, lightDirection), 0.0);
    
    // calc diffuse for back-facing surfaces (inverted normal)
    float diffBack = max(dot(-norm, lightDirection), 0.0);
    
    // combine front & back lighting for transparency effect
    float totalDiff = diffFront + (diffBack * 0.6); 
    vec3 diffuse = totalDiff * lightColor;
    
    // combine results with higher transparency
    vec3 result = (vec3(0.1) + diffuse) * objectColor; // objectColor should be yellow
    FragColor = vec4(result, 0.3); 
}
