#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Instance matrix — one per tree (mat4 spans locations 3–6)
layout (location = 3) in mat4 instanceMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Vertices are already in mini-tree local space (branches baked in).
    // instanceMatrix places each tree in world space.
    vec4 worldPos = instanceMatrix * vec4(aPos, 1.0);

    gl_Position = projection * view * worldPos;
    FragPos     = vec3(worldPos);
    TexCoord    = aTexCoord;

    // Normal transform — safe for translate + uniform-scale instance matrices
    Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
}
