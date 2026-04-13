#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

// Horizon fog — hazes the bottom of the skybox to blend with scene fog
uniform bool  horizonFogEnabled;
uniform vec3  horizonFogColor;
uniform float horizonFogStart;   // Y value where haze begins (e.g. 0.15)
uniform float horizonFogEnd;     // Y value where haze is fully opaque (e.g. -0.05)

void main()
{
    vec4 skyColor = texture(skybox, texCoords);

    if (horizonFogEnabled) {
        // Normalize the Y component of the sampling direction
        float y = normalize(texCoords).y;
        // Smoothly blend from sky to fog as Y approaches horizon
        float fogFactor = 1.0 - smoothstep(horizonFogEnd, horizonFogStart, y);
        skyColor = mix(skyColor, vec4(horizonFogColor, 1.0), fogFactor);
    }

    FragColor = skyColor;
}