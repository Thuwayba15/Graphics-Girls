#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    vec3 ambient;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;

void main()
{
    // simple lighting calc.
    vec3 lightPos = vec3(5.0, 8.0, 5.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 ambientColor = vec3(0.3, 0.3, 0.3);
    
    // ambient
    vec3 ambient = ambientColor * color;
    
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * color;
    
    // specular
    vec3 viewDir = normalize(-FragPos); // Camera is at origin
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor * 0.5;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
