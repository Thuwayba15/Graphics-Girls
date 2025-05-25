#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 8  // match this w. the no, of lights

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform bool useTexture;
uniform float alpha;
uniform sampler2D texture1;

out vec4 FragColor;

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    
    // calc all point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);
    
    // ambient contribution
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * objectColor;
    result += ambient;

    // combine w. texture
    vec3 baseColor = useTexture ? texture(texture1, TexCoord).rgb : objectColor;
    FragColor = vec4(baseColor * result, alpha);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;
    
    // specular
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.color;
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    return (diffuse + specular) * attenuation;
}
