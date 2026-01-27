#version 330 core
out vec4 FragColor;

uniform vec4 objectColor;
uniform bool useLight;
uniform vec3 lightColor;

void main()
{
    if (useLight) {
        FragColor = vec4(objectColor.rgb * lightColor, objectColor.a);
    } else {
        FragColor = objectColor;
    }
}
