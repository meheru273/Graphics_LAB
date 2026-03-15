#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;
    float k_l;
    float k_q;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;
    float k_l;
    float k_q;
};

#define NR_POINT_LIGHTS 2

uniform vec3 viewPos;
uniform Material material;

uniform DirLight dirLight;
uniform bool dirLightOn;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool pointLightOn;

uniform SpotLight spotLight;
uniform bool spotLightOn;

// component toggles
uniform bool ambientOn;
uniform bool diffuseOn;
uniform bool specularOn;

vec3 CalcDirLight(DirLight light, vec3 N, vec3 V);
vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 V);
vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 fragPos, vec3 V);

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    if (dirLightOn)
        result += CalcDirLight(dirLight, N, V);

    if (pointLightOn) {
        for (int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcPointLight(pointLights[i], N, FragPos, V);
    }

    if (spotLightOn)
        result += CalcSpotLight(spotLight, N, FragPos, V);

    // if no lights on, show a dim ambient
    if (!dirLightOn && !pointLightOn && !spotLightOn)
        result = material.ambient * 0.1;

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 N, vec3 V)
{
    vec3 L = normalize(-light.direction);
    vec3 R = reflect(-L, N);

    vec3 ambient  = ambientOn  ? material.ambient  * light.ambient  : vec3(0.0);
    vec3 diffuse  = diffuseOn  ? material.diffuse  * max(dot(N, L), 0.0) * light.diffuse  : vec3(0.0);
    vec3 specular = specularOn ? material.specular * pow(max(dot(V, R), 0.0), material.shininess) * light.specular : vec3(0.0);

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);

    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * d * d);

    vec3 ambient  = ambientOn  ? material.ambient  * light.ambient  : vec3(0.0);
    vec3 diffuse  = diffuseOn  ? material.diffuse  * max(dot(N, L), 0.0) * light.diffuse  : vec3(0.0);
    vec3 specular = specularOn ? material.specular * pow(max(dot(V, R), 0.0), material.shininess) * light.specular : vec3(0.0);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);

    float theta = dot(L, normalize(-light.direction));

    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * d * d);

    vec3 ambient  = ambientOn  ? material.ambient  * light.ambient  : vec3(0.0);
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (theta > light.cutOff) {
        diffuse  = diffuseOn  ? material.diffuse  * max(dot(N, L), 0.0) * light.diffuse  : vec3(0.0);
        specular = specularOn ? material.specular * pow(max(dot(V, R), 0.0), material.shininess) * light.specular : vec3(0.0);
    }

    return (ambient + diffuse + specular) * attenuation;
}
