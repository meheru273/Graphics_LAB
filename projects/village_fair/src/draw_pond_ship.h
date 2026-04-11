// draw_pond_ship.h — Combined pond + pirate ship scene.
// Everything is in local space (origin = pond center).
// Place anywhere with a single moveMatrix.
// #included from main.cpp; inherits all globals.
#pragma once

#include <vector>
#include <cmath>

// ──────────────────────────────────────────────────────────────
//  Elliptical bezier pond mesh (built once at init)
// ──────────────────────────────────────────────────────────────
struct PondMesh {
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int          indexCount = 0;

    void build(float radiusX, float radiusZ, int rings, int slices)
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        // Center vertex
        vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

        for (int r = 1; r <= rings; ++r) {
            float t = (float)r / rings;
            for (int s = 0; s < slices; ++s) {
                float theta = 2.0f * 3.14159265f * s / slices;
                float x = t * radiusX * cosf(theta);
                float z = t * radiusZ * sinf(theta);
                vertices.push_back(x);  vertices.push_back(0.0f); vertices.push_back(z);
                vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
            }
        }

        // Center fan
        for (int s = 0; s < slices; ++s) {
            int next = (s + 1) % slices;
            indices.push_back(0);
            indices.push_back(1 + s);
            indices.push_back(1 + next);
        }
        // Ring-to-ring quads
        for (int r = 1; r < rings; ++r) {
            int base0 = 1 + (r - 1) * slices;
            int base1 = 1 + r * slices;
            for (int s = 0; s < slices; ++s) {
                int next = (s + 1) % slices;
                indices.push_back(base0 + s);
                indices.push_back(base1 + s);
                indices.push_back(base1 + next);
                indices.push_back(base0 + s);
                indices.push_back(base1 + next);
                indices.push_back(base0 + next);
            }
        }

        indexCount = (int)indices.size();

        glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void updateWaves(float time, float radiusX, float radiusZ, int rings, int slices)
    {
        std::vector<float> vertices;
        float cy = 0.02f * sinf(time * 2.0f);
        vertices.push_back(0.0f); vertices.push_back(cy); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

        for (int r = 1; r <= rings; ++r) {
            float t = (float)r / rings;
            for (int s = 0; s < slices; ++s) {
                float theta = 2.0f * 3.14159265f * s / slices;
                float x = t * radiusX * cosf(theta);
                float z = t * radiusZ * sinf(theta);
                float wave = t * 0.08f * sinf(x * 1.5f + time * 3.0f)
                           + t * 0.05f * sinf(z * 2.0f + time * 2.5f + 1.3f)
                           + t * 0.03f * cosf((x + z) * 1.0f + time * 4.0f);
                vertices.push_back(x); vertices.push_back(wave); vertices.push_back(z);
                float nx = -t * 0.08f * 1.5f * cosf(x * 1.5f + time * 3.0f);
                float nz = -t * 0.05f * 2.0f * cosf(z * 2.0f + time * 2.5f + 1.3f);
                float len = sqrtf(nx * nx + 1.0f + nz * nz);
                vertices.push_back(nx / len); vertices.push_back(1.0f / len); vertices.push_back(nz / len);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    }
};

// ──────────────────────────────────────────────────────────────
//  Global pond+ship state
// ──────────────────────────────────────────────────────────────
PondMesh pondMesh;
static const float POND_RX    = 6.5f;
static const float POND_RZ    = 4.0f;
static const int   POND_RINGS = 20;
static const int   POND_SLICES = 40;

void initPondShip() {
    pondMesh.build(POND_RX, POND_RZ, POND_RINGS, POND_SLICES);
}

// ──────────────────────────────────────────────────────────────
//  Draw pond water surface
// ──────────────────────────────────────────────────────────────
static void _drawPondWater(Shader& shader, glm::mat4 moveMatrix, float time)
{
    pondMesh.updateWaves(time, POND_RX, POND_RZ, POND_RINGS, POND_SLICES);
    shader.setMat4("model", moveMatrix);
    float shimmer = 0.04f * sinf(time * 1.5f);
    glm::vec4 waterColor(0.05f + shimmer, 0.25f + shimmer, 0.75f - shimmer, 1.0f);
    shader.setVec4("material.ambient",  waterColor * 0.4f);
    shader.setVec4("material.diffuse",  waterColor);
    shader.setVec4("material.specular", glm::vec4(0.8f, 0.8f, 0.9f, 1.0f));
    shader.setFloat("material.shininess", 96.0f);
    glBindVertexArray(pondMesh.VAO);
    glDrawElements(GL_TRIANGLES, pondMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Draw wooden deck at pond edge
// ──────────────────────────────────────────────────────────────
static void _drawDeck(Shader& shader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, model;
    glm::vec4 woodColor(0.45f, 0.28f, 0.12f, 1.0f);

    // Main platform
    T = glm::translate(I, glm::vec3(-2.0f, 0.05f, POND_RZ - 0.5f));
    S = glm::scale(I, glm::vec3(8.0f, 0.15f, 3.0f));
    model = T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient",  woodColor * 0.5f);
    shader.setVec4("material.diffuse",  woodColor);
    shader.setVec4("material.specular", glm::vec4(0.2f, 0.15f, 0.1f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Posts
    glm::vec4 postColor(0.35f, 0.20f, 0.08f, 1.0f);
    float px[] = { -1.8f, 1.8f, -1.8f, 1.8f };
    float pz[] = { POND_RZ - 0.3f, POND_RZ - 0.3f, POND_RZ + 0.8f, POND_RZ + 0.8f };
    for (int i = 0; i < 4; ++i) {
        T = glm::translate(I, glm::vec3(px[i], -0.42f, pz[i]));
        S = glm::scale(I, glm::vec3(0.2f, 1.0f, 0.2f));
        shader.setMat4("model", moveMatrix * T * S);
        shader.setVec4("material.ambient",  postColor * 0.5f);
        shader.setVec4("material.diffuse",  postColor);
        shader.setVec4("material.specular", glm::vec4(0.15f));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Railings
    glm::vec4 railColor(0.50f, 0.32f, 0.15f, 1.0f);
    shader.setVec4("material.ambient",  railColor * 0.5f);
    shader.setVec4("material.diffuse",  railColor);
    // Left
    T = glm::translate(I, glm::vec3(-2.0f, 0.15f, POND_RZ - 0.3f));
    S = glm::scale(I, glm::vec3(0.1f, 0.6f, 3.0f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Right
    T = glm::translate(I, glm::vec3(1.9f, 0.15f, POND_RZ - 0.3f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Front
    T = glm::translate(I, glm::vec3(-2.0f, 0.15f, POND_RZ - 0.3f));
    S = glm::scale(I, glm::vec3(8.0f, 0.6f, 0.1f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Pirate Ship — recentered to local origin (0,0,0)
//  Original pivot was (14, 2, 4.2), now at (0, 2, 0).
//  All coordinates shifted by (-14, 0, -4.2)
// ──────────────────────────────────────────────────────────────
static void _drawPirateShip(Shader& shader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, model, rotZ, rotTemp;

    // Pivot at local (0, 2, 0)
    glm::vec4 pivot = glm::vec4(0.0f, 2.0f, 0.0f, 1.0f);
    glm::mat4 toPivot   = glm::translate(I, glm::vec3(-pivot));
    glm::mat4 fromPivot = glm::translate(I, glm::vec3(pivot));

    // Animation — same logic as original PirateShip
    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastTime > 0.1)
    {
        if (isBoatOn)
            boatAngle += boatSpeed;
        if (abs(boatAngle) >= boatMaxAngle)
        {
            boatSpeed *= -1;
            lastTime = static_cast<float>(glfwGetTime());
        }
    }
    if (!isBoatOn)
    {
        if (boatAngle < 0) boatAngle += abs(boatSpeed);
        if (boatAngle > 0) boatAngle -= abs(boatSpeed);
    }

    rotZ = glm::rotate(I, glm::radians(boatAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    // ---- Boat hull (bezier mesh) ----
    // Original: (14, -1.18, 7) → local: (0, -1.18, 2.8)
    glm::vec4 color = glm::vec4(1.0f, 10.0f/256, 20.0f/256, 1.0f);
    glBindTexture(GL_TEXTURE_2D, texture3);
    T = glm::translate(I, glm::vec3(0.0f, -1.18f, 2.8f));
    S = glm::scale(I, glm::vec3(0.7f, 0.7f, 0.3f));
    model = fromPivot * rotZ * toPivot * T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient", color);
    shader.setVec4("material.diffuse", color * 0.5f);
    shader.setVec4("material.specular", color);
    shader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    // ---- Base (under the hull) ----
    // Original: (14, -0.45, 7) → local: (0, -0.45, 2.8)
    color = glm::vec4(150.0f/256, 12.0f/256, 35.0f/256, 1.0f);
    T = glm::translate(I, glm::vec3(0.0f, -0.45f, 2.8f));
    S = glm::scale(I, glm::vec3(0.55f, 0.2f, 0.248f));
    model = fromPivot * rotZ * toPivot * T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient", color);
    shader.setVec4("material.diffuse", color * 0.5f);
    shader.setVec4("material.specular", color);
    shader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    // ---- Support bars ----
    // Original bars offset: moveX=11, moveY=-0.4, moveZ=7.7
    // Bar base coords: (2.975+11, 0.5-0.4, -1.2+7.7) = (13.975, 0.1, 6.5) → local: (-0.025, 0.1, 2.3)
    glBindTexture(GL_TEXTURE_2D, texture0);
    color = glm::vec4(1.0f);
    shader.setVec4("material.ambient", color);
    shader.setVec4("material.diffuse", color);
    shader.setVec4("material.specular", color);
    shader.setFloat("material.shininess", 256.0f);

    // Offsets: original (moveX, moveY, moveZ) = (11, -0.4, 7.7)
    // Shift: subtract (14, 0, 4.2) from final coords
    float oX = -3.0f, oY = -0.4f, oZ = 3.5f; // = (11-14, -0.4, 7.7-4.2)
    // Bar 1
    T = glm::translate(I, glm::vec3(2.975f + oX, 0.5f + oY, -1.2f + oZ));
    S = glm::scale(I, glm::vec3(0.1f, 0.1f, 1.95f));
    model = fromPivot * rotZ * toPivot * T * S;
    shader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Bar 2
    T = glm::translate(I, glm::vec3(2.575f + oX, 0.5f + oY, -1.2f + oZ));
    model = fromPivot * rotZ * toPivot * T * S;
    shader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Bar 3
    T = glm::translate(I, glm::vec3(3.375f + oX, 0.5f + oY, -1.2f + oZ));
    model = fromPivot * rotZ * toPivot * T * S;
    shader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Small dock boat (bobbing at the edge, with mast and flag)
// ──────────────────────────────────────────────────────────────
static void _drawDockBoat(Shader& shader, glm::mat4 moveMatrix, float time)
{
    glm::mat4 I = glm::mat4(1.0f);

    float boatX = 0.0f;
    float boatZ = POND_RZ - 1.5f;
    float bobY  = 0.04f * sinf(time * 2.5f) + 0.02f * sinf(time * 1.7f + 0.5f);
    float roll  = 3.0f * sinf(time * 1.8f + 0.3f);
    float yaw   = 2.0f * sinf(time * 0.7f);

    glm::mat4 boatModel = glm::translate(I, glm::vec3(boatX, -0.25f + bobY, boatZ));
    boatModel = glm::rotate(boatModel, glm::radians(yaw), glm::vec3(0,1,0));
    boatModel = glm::rotate(boatModel, glm::radians(roll), glm::vec3(0,0,1));

    // Hull
    glm::vec4 hullColor(0.6f, 0.15f, 0.05f, 1.0f);
    glm::mat4 hullS = glm::scale(I, glm::vec3(0.5f, 0.4f, 0.25f));
    shader.setMat4("model", moveMatrix * boatModel * hullS);
    shader.setVec4("material.ambient",  hullColor * 0.4f);
    shader.setVec4("material.diffuse",  hullColor);
    shader.setVec4("material.specular", glm::vec4(0.3f, 0.2f, 0.1f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    glBindVertexArray(cubeVAO);
    // Seat plank
    glm::vec4 seatC(0.55f, 0.35f, 0.15f, 1.0f);
    shader.setMat4("model", moveMatrix * boatModel *
        glm::translate(I, glm::vec3(-0.3f, 0.1f, -0.15f)) *
        glm::scale(I, glm::vec3(1.2f, 0.08f, 0.6f)));
    shader.setVec4("material.ambient",  seatC * 0.5f);
    shader.setVec4("material.diffuse",  seatC);
    shader.setVec4("material.specular", glm::vec4(0.1f));
    shader.setFloat("material.shininess", 16.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Mast
    glm::vec4 mastC(0.4f, 0.25f, 0.1f, 1.0f);
    shader.setMat4("model", moveMatrix * boatModel *
        glm::translate(I, glm::vec3(0.0f, 0.1f, -0.05f)) *
        glm::scale(I, glm::vec3(0.06f, 1.8f, 0.06f)));
    shader.setVec4("material.ambient",  mastC * 0.5f);
    shader.setVec4("material.diffuse",  mastC);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Flag
    float flagWave = 0.15f * sinf(time * 5.0f);
    glm::vec4 flagC(0.9f, 0.15f, 0.1f, 1.0f);
    shader.setMat4("model", moveMatrix * boatModel *
        glm::translate(I, glm::vec3(0.03f, 0.95f, -0.05f)) *
        glm::scale(I, glm::vec3(0.5f + flagWave, 0.25f, 0.02f)));
    shader.setVec4("material.ambient",  flagC * 0.5f);
    shader.setVec4("material.diffuse",  flagC);
    shader.setVec4("material.specular", glm::vec4(0.1f));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Rope
    glm::vec4 ropeC(0.6f, 0.55f, 0.35f, 1.0f);
    shader.setMat4("model", moveMatrix * boatModel *
        glm::translate(I, glm::vec3(0.3f, 0.05f, 0.6f)) *
        glm::scale(I, glm::vec3(0.03f, 0.03f, 1.5f)));
    shader.setVec4("material.ambient",  ropeC * 0.5f);
    shader.setVec4("material.diffuse",  ropeC);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  PUBLIC: Draw everything — pond + pirate ship + deck + dock boat
//  One moveMatrix controls the entire scene.
// ──────────────────────────────────────────────────────────────
void drawPondShipScene(Shader& shader, glm::mat4 moveMatrix, float time)
{
    // 1. Animated water surface
    _drawPondWater(shader, moveMatrix, time);

    // 2. Pirate ship (swinging, in the middle of the pond)
    glBindTexture(GL_TEXTURE_2D, texture3);
    _drawPirateShip(shader, moveMatrix);

    // 3. Wooden deck
    glBindTexture(GL_TEXTURE_2D, texture3);
    _drawDeck(shader, moveMatrix);

    // 4. Small dock boat (bobbing at edge)
    glBindTexture(GL_TEXTURE_2D, texture0);
    _drawDockBoat(shader, moveMatrix, time);
}
