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
        // Centre vertex — small bob only, no wave gaps since there's nothing
        // to line up with at the centre.
        float cy = 0.02f * sinf(time * 2.0f);
        vertices.push_back(0.0f); vertices.push_back(cy); vertices.push_back(0.0f);
        vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

        for (int r = 1; r <= rings; ++r) {
            float t = (float)r / rings;
            // Envelope that is 1 at the centre and 0 at the edge, so the
            // outermost ring of vertices does not move and can't open up
            // gaps between the pond surface and the surrounding ground /
            // boundary ring.
            float envelope = (1.0f - t) * (1.0f - t);
            for (int s = 0; s < slices; ++s) {
                float theta = 2.0f * 3.14159265f * s / slices;
                float x = t * radiusX * cosf(theta);
                float z = t * radiusZ * sinf(theta);
                float wave = envelope * (
                      0.08f * sinf(x * 1.5f + time * 3.0f)
                    + 0.05f * sinf(z * 2.0f + time * 2.5f + 1.3f)
                    + 0.03f * cosf((x + z) * 1.0f + time * 4.0f));
                vertices.push_back(x); vertices.push_back(wave); vertices.push_back(z);
                float nx = -envelope * 0.08f * 1.5f * cosf(x * 1.5f + time * 3.0f);
                float nz = -envelope * 0.05f * 2.0f * cosf(z * 2.0f + time * 2.5f + 1.3f);
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
    // Semi-transparent water (alpha = 0.72) so submerged hull is visible
    glm::vec4 waterColor(0.05f + shimmer, 0.25f + shimmer, 0.75f - shimmer, 0.72f);
    shader.setVec4("material.ambient",  glm::vec4(glm::vec3(waterColor) * 0.4f, waterColor.a));
    shader.setVec4("material.diffuse",  waterColor);
    shader.setVec4("material.specular", glm::vec4(0.8f, 0.8f, 0.9f, waterColor.a));
    shader.setFloat("material.shininess", 96.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);  // don't write to depth buffer for transparency

    glBindVertexArray(pondMesh.VAO);
    glDrawElements(GL_TRIANGLES, pondMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

// ──────────────────────────────────────────────────────────────
//  Draw wooden deck at pond edge.
//
//  Placed on the -Z side of the pond (the tree side). All Z
//  coordinates are the mirror of the original +Z layout:
//    platform:  Z ∈ [-5.0, -3.5]  (straddles pond -Z edge at -4)
//    posts:     Z = -3.7 (pond-facing) and -4.8 (outer)
//    rails:     mirrored to match
// ──────────────────────────────────────────────────────────────
static void _drawDeck(Shader& shader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, model;
    glm::vec4 woodColor(0.45f, 0.28f, 0.12f, 1.0f);

    // Main platform — spans X ∈ [-2, 2], Z ∈ [-5.0, -3.5].
    T = glm::translate(I, glm::vec3(-2.0f, 0.05f, -(POND_RZ + 1.0f)));
    S = glm::scale(I, glm::vec3(8.0f, 0.15f, 3.0f));
    model = T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient",  woodColor * 0.5f);
    shader.setVec4("material.diffuse",  woodColor);
    shader.setVec4("material.specular", glm::vec4(0.2f, 0.15f, 0.1f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Posts — two at the pond-facing edge (Z = -3.7), two at the outer
    // edge (Z = -4.8).
    glm::vec4 postColor(0.35f, 0.20f, 0.08f, 1.0f);
    float px[] = { -1.8f, 1.8f, -1.8f, 1.8f };
    float pz[] = { -(POND_RZ - 0.3f), -(POND_RZ - 0.3f),
                   -(POND_RZ + 0.8f), -(POND_RZ + 0.8f) };
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
    // Left side rail — runs along -Z at X = -2
    T = glm::translate(I, glm::vec3(-2.0f, 0.15f, -(POND_RZ + 1.2f)));
    S = glm::scale(I, glm::vec3(0.1f, 0.6f, 3.0f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Right side rail
    T = glm::translate(I, glm::vec3(1.9f, 0.15f, -(POND_RZ + 1.2f)));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // Far-edge rail (away from pond, at Z = -5.0)
    T = glm::translate(I, glm::vec3(-2.0f, 0.15f, -(POND_RZ + 1.05f)));
    S = glm::scale(I, glm::vec3(8.0f, 0.6f, 0.1f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Pirate Ship ride.
//
//  Structure:
//    • A static A-frame (two tall vertical posts + a horizontal
//      crossbar at the top) straddles the ship on the X axis. It
//      does NOT swing.
//    • Two hanging arms drop from the crossbar down to the hull.
//      They swing around the pivot line at y = 2.2 along with the
//      hull / base, giving the visual of the ship being held by
//      the frame from above.
//    • Hull and base hang at the bottom of the arms.
//
//  Pivot is (0, 2.2, *) — the Z component doesn't matter because
//  we rotate around the Z axis. All coordinates are in pond-local
//  space; the caller applies moveMatrix.
// ──────────────────────────────────────────────────────────────
static void _drawPirateShip(Shader& shader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, model;

    // Pivot axis at local (0, 2.2, 0)
    glm::vec4 pivot = glm::vec4(0.0f, 2.2f, 0.0f, 1.0f);
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

    glm::mat4 rotZ  = glm::rotate(I, glm::radians(boatAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 swing = fromPivot * rotZ * toPivot;

    // ── STATIC A-FRAME ─────────────────────────────────────────────
    // These three cubes do NOT get the swing transform — the frame
    // stays still while the ship rocks inside it.
    glBindTexture(GL_TEXTURE_2D, texture0);
    glBindVertexArray(cubeVAO);
    glm::vec4 frameColor(0.45f, 0.28f, 0.12f, 1.0f);
    shader.setVec4("material.ambient",  frameColor * 0.5f);
    shader.setVec4("material.diffuse",  frameColor);
    shader.setVec4("material.specular", glm::vec4(0.2f, 0.15f, 0.1f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);

    // Left vertical post — size 0.2 × 2.62 × 0.2, centred at (-2.5, 0.89, 2.8)
    T = glm::translate(I, glm::vec3(-2.6f, -0.42f, 2.7f));
    S = glm::scale(I, glm::vec3(0.4f, 5.24f, 0.4f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right vertical post — size 0.2 × 2.62 × 0.2, centred at (2.5, 0.89, 2.8)
    T = glm::translate(I, glm::vec3(2.4f, -0.42f, 2.7f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Top crossbar — size 5.4 × 0.15 × 0.15, centred at (0, 2.275, 2.8)
    T = glm::translate(I, glm::vec3(-2.7f, 2.2f, 2.725f));
    S = glm::scale(I, glm::vec3(10.8f, 0.3f, 0.3f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── SWINGING HANGING ARMS ──────────────────────────────────────
    // Size 0.08 × 3.2 × 0.08. Top at y = 2.2 (pivot line), bottom at
    // y = -1.0 — so the top sits inside the crossbar and the bottom
    // reaches the hull below.
    glm::vec4 armColor(0.55f, 0.35f, 0.15f, 1.0f);
    shader.setVec4("material.ambient",  armColor * 0.5f);
    shader.setVec4("material.diffuse",  armColor);
    shader.setVec4("material.specular", glm::vec4(0.2f));
    shader.setFloat("material.shininess", 32.0f);

    // Left arm — centred at (-0.4, 0.6, 2.8)
    T = glm::translate(I, glm::vec3(-0.44f, -1.0f, 2.76f));
    S = glm::scale(I, glm::vec3(0.16f, 6.4f, 0.16f));
    model = swing * T * S;
    shader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right arm — centred at (0.4, 0.6, 2.8)
    T = glm::translate(I, glm::vec3(0.36f, -1.0f, 2.76f));
    model = swing * T * S;
    shader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── BOAT HULL (bezier mesh) ────────────────────────────────────
    // Hangs below the arms and swings with them.
    glm::vec4 color(1.0f, 10.0f/256, 20.0f/256, 1.0f);
    glBindTexture(GL_TEXTURE_2D, texture3);
    T = glm::translate(I, glm::vec3(0.0f, -1.18f, 2.8f));
    S = glm::scale(I, glm::vec3(0.7f, 0.7f, 0.3f));
    model = swing * T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient",  color);
    shader.setVec4("material.diffuse",  color * 0.5f);
    shader.setVec4("material.specular", color);
    shader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);

    // ── Base (under the hull) ─────────────────────────────────────
    color = glm::vec4(150.0f/256, 12.0f/256, 35.0f/256, 1.0f);
    T = glm::translate(I, glm::vec3(0.0f, -0.45f, 2.8f));
    S = glm::scale(I, glm::vec3(0.55f, 0.2f, 0.248f));
    model = swing * T * S;
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient",  color);
    shader.setVec4("material.diffuse",  color * 0.5f);
    shader.setVec4("material.specular", color);
    shader.setFloat("material.shininess", 256.0f);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Small dock boat (bobbing at the edge, with mast and flag)
// ──────────────────────────────────────────────────────────────
static void _drawDockBoat(Shader& shader, glm::mat4 moveMatrix, float time)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S;

    // ── Centered in the pond, floating at water level ────────────
    float bobY = 0.03f * sinf(time * 2.5f) + 0.015f * sinf(time * 1.7f + 0.5f);
    float roll = 1.5f * sinf(time * 1.8f + 0.3f);
    float yaw  = 1.2f * sinf(time * 0.7f);

    // Boat floating ON the water — only hull bottom dips below surface
    glm::mat4 boatModel = glm::translate(I, glm::vec3(0.0f, 0.08f + bobY, 0.0f));
    boatModel = glm::rotate(boatModel, glm::radians(yaw), glm::vec3(0,1,0));
    boatModel = glm::rotate(boatModel, glm::radians(roll), glm::vec3(0,0,1));

    // Boat dimensions — elongated hull (length >> width)
    const float HULL_LEN = 4.0f;    // long boat
    const float HULL_WID = 1.0f;    // narrow
    const float HULL_H   = 0.15f;   // shallow draft (only this goes underwater)
    const float WALL_H   = 0.35f;
    const float WALL_T   = 0.06f;
    const float FRAME_T  = 0.04f;

    glBindVertexArray(cubeVAO);

    // ── HULL BOTTOM (submerged under water) ─────────────────────
    glm::vec4 hullColor(0.50f, 0.14f, 0.04f, 1.0f);
    shader.setVec4("material.ambient",  hullColor * 0.4f);
    shader.setVec4("material.diffuse",  hullColor);
    shader.setVec4("material.specular", glm::vec4(0.20f, 0.12f, 0.06f, 1.0f));
    shader.setFloat("material.shininess", 24.0f);

    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.5f, -HULL_H, -HULL_WID * 0.5f));
    S = glm::scale(I, glm::vec3(HULL_LEN / 0.5f, HULL_H / 0.5f, HULL_WID / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── FLOOR / DECK (covers the inside, no gap!) ───────────────
    glm::vec4 floorColor(0.48f, 0.30f, 0.12f, 1.0f);
    shader.setVec4("material.ambient",  floorColor * 0.45f);
    shader.setVec4("material.diffuse",  floorColor);
    shader.setVec4("material.specular", glm::vec4(0.10f));
    shader.setFloat("material.shininess", 10.0f);

    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.5f + WALL_T, -0.02f, -HULL_WID * 0.5f + WALL_T));
    S = glm::scale(I, glm::vec3((HULL_LEN - 2.0f * WALL_T) / 0.5f,
                                 0.04f / 0.5f,
                                 (HULL_WID - 2.0f * WALL_T) / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── SIDE WALLS ────────────────────────────────────────────
    glm::vec4 wallColor(0.55f, 0.30f, 0.10f, 1.0f);
    shader.setVec4("material.ambient",  wallColor * 0.4f);
    shader.setVec4("material.diffuse",  wallColor);
    shader.setVec4("material.specular", glm::vec4(0.12f, 0.08f, 0.04f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);

    // Left wall (-Z side)
    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.5f, 0.0f, -HULL_WID * 0.5f));
    S = glm::scale(I, glm::vec3(HULL_LEN / 0.5f, WALL_H / 0.5f, WALL_T / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right wall (+Z side)
    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.5f, 0.0f, HULL_WID * 0.5f - WALL_T));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Back wall (+X side, stern) — full width
    T = glm::translate(I, glm::vec3(HULL_LEN * 0.5f - WALL_T, 0.0f, -HULL_WID * 0.5f));
    S = glm::scale(I, glm::vec3(WALL_T / 0.5f, WALL_H / 0.5f, HULL_WID / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // BOW (tapered front) — two angled side pieces to create a V-shape
    // Left bow piece
    glm::mat4 bowL = boatModel *
        glm::translate(I, glm::vec3(-HULL_LEN * 0.5f, 0.0f, -HULL_WID * 0.15f)) *
        glm::rotate(I, glm::radians(-25.0f), glm::vec3(0,1,0));
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    S = glm::scale(I, glm::vec3(WALL_T / 0.5f, WALL_H / 0.5f, (HULL_WID * 0.45f) / 0.5f));
    shader.setMat4("model", moveMatrix * bowL * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right bow piece
    glm::mat4 bowR = boatModel *
        glm::translate(I, glm::vec3(-HULL_LEN * 0.5f, 0.0f, HULL_WID * 0.15f)) *
        glm::rotate(I, glm::radians(25.0f), glm::vec3(0,1,0));
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, -HULL_WID * 0.45f + WALL_T));
    S = glm::scale(I, glm::vec3(WALL_T / 0.5f, WALL_H / 0.5f, (HULL_WID * 0.45f) / 0.5f));
    shader.setMat4("model", moveMatrix * bowR * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── FRAME BEAMS (top rail around the boat) ─────────────────
    glm::vec4 frameColor(0.40f, 0.22f, 0.08f, 1.0f);
    shader.setVec4("material.ambient",  frameColor * 0.5f);
    shader.setVec4("material.diffuse",  frameColor);
    shader.setVec4("material.specular", glm::vec4(0.10f));
    shader.setFloat("material.shininess", 10.0f);

    // Long beams (along X, top of left/right walls)
    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.35f, WALL_H, -HULL_WID * 0.5f));
    S = glm::scale(I, glm::vec3((HULL_LEN * 0.85f) / 0.5f, FRAME_T / 0.5f, FRAME_T / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    T = glm::translate(I, glm::vec3(-HULL_LEN * 0.35f, WALL_H, HULL_WID * 0.5f - FRAME_T));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Back beam (stern, along Z)
    T = glm::translate(I, glm::vec3(HULL_LEN * 0.5f - FRAME_T, WALL_H, -HULL_WID * 0.5f));
    S = glm::scale(I, glm::vec3(FRAME_T / 0.5f, FRAME_T / 0.5f, HULL_WID / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── TWO BENCHES (cross-planks inside) ─────────────────────
    glm::vec4 benchColor(0.50f, 0.33f, 0.14f, 1.0f);
    shader.setVec4("material.ambient",  benchColor * 0.5f);
    shader.setVec4("material.diffuse",  benchColor);
    shader.setVec4("material.specular", glm::vec4(0.08f));
    shader.setFloat("material.shininess", 8.0f);

    float benchW = HULL_WID - 2.0f * WALL_T - 0.10f;
    // Bench near middle
    T = glm::translate(I, glm::vec3(-0.15f, 0.02f, -benchW * 0.5f));
    S = glm::scale(I, glm::vec3(0.30f / 0.5f, 0.10f / 0.5f, benchW / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Bench near stern
    T = glm::translate(I, glm::vec3(HULL_LEN * 0.30f, 0.02f, -benchW * 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── MAST (thin pole, 25% of previous thickness) ─────────────
    glm::vec4 mastC(0.35f, 0.20f, 0.08f, 1.0f);
    shader.setVec4("material.ambient",  mastC * 0.5f);
    shader.setVec4("material.diffuse",  mastC);
    T = glm::translate(I, glm::vec3(-0.015f, 0.02f, -0.015f));
    S = glm::scale(I, glm::vec3(0.06f / 0.5f, 1.6f / 0.5f, 0.06f / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── FLAG (thin fabric-like quad) ───────────────────────────
    float flagWave = 0.08f * sinf(time * 5.0f);
    glm::vec4 flagC(0.85f, 0.12f, 0.08f, 1.0f);
    shader.setVec4("material.ambient",  flagC * 0.5f);
    shader.setVec4("material.diffuse",  flagC);
    shader.setVec4("material.specular", glm::vec4(0.05f));
    T = glm::translate(I, glm::vec3(0.03f, 1.35f, -0.005f));
    S = glm::scale(I, glm::vec3((0.45f + flagWave) / 0.5f, 0.22f / 0.5f, 0.01f / 0.5f));
    shader.setMat4("model", moveMatrix * boatModel * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Pond boundary — a ring of stone-coloured cuboids placed tangent
//  to the pond ellipse. Hides any residual gaps between the water
//  surface and the surrounding terrain, and gives the pond a
//  visible rim.
// ──────────────────────────────────────────────────────────────
static void _drawPondRim(Shader& shader, glm::mat4 moveMatrix)
{
    const glm::mat4 I = glm::mat4(1.0f);
    const int   N        = 48;     // ring segments
    const float rimH     = 0.50f;  // height
    const float rimT     = 0.30f;  // thickness (inward/outward)
    const float rimYMid  = 0.05f;  // vertical centre (local Y)

    glBindVertexArray(cubeVAO);
    glm::vec4 rimColor(0.38f, 0.30f, 0.22f, 1.0f); // dark stone / dirt
    shader.setVec4("material.ambient",  rimColor * 0.5f);
    shader.setVec4("material.diffuse",  rimColor);
    shader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);

    for (int i = 0; i < N; ++i) {
        float t0 = 2.0f * 3.14159265f * (float)i       / (float)N;
        float t1 = 2.0f * 3.14159265f * (float)(i + 1) / (float)N;
        float x0 = POND_RX * cosf(t0), z0 = POND_RZ * sinf(t0);
        float x1 = POND_RX * cosf(t1), z1 = POND_RZ * sinf(t1);
        float mx = 0.5f * (x0 + x1);
        float mz = 0.5f * (z0 + z1);
        float dx = x1 - x0;
        float dz = z1 - z0;
        // Segment length gets a small overlap so neighbouring pieces
        // meet with no visible seam.
        float segLen = sqrtf(dx * dx + dz * dz) * 1.10f;
        // Rotation around +Y so the cube's local +X aligns with the
        // tangent (dx, dz). glm::rotate around (0,1,0) maps +X to
        // (cosθ, 0, -sinθ), so we need θ = atan2(-dz, dx).
        float angleY = atan2f(-dz, dx);

        glm::mat4 M = glm::translate(I, glm::vec3(mx, rimYMid, mz))
                    * glm::rotate(I, angleY, glm::vec3(0.0f, 1.0f, 0.0f))
                    * glm::translate(I, glm::vec3(-segLen * 0.5f,
                                                  -rimH   * 0.5f,
                                                  -rimT   * 0.5f))
                    * glm::scale(I, glm::vec3(2.0f * segLen,
                                              2.0f * rimH,
                                              2.0f * rimT));
        shader.setMat4("model", moveMatrix * M);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  PUBLIC: Draw everything — pond + rim + pirate ship + deck + dock boat
//  One moveMatrix controls the entire scene.
// ──────────────────────────────────────────────────────────────
void drawPondShipScene(Shader& shader, glm::mat4 moveMatrix, float time)
{
    // Draw ORDER matters for transparency:
    // 1. All opaque geometry first
    // 2. Semi-transparent water last

    // 1. Stone rim (opaque)
    glBindTexture(GL_TEXTURE_2D, texture0);
    _drawPondRim(shader, moveMatrix);

    // 2. Wooden deck (opaque)
    glBindTexture(GL_TEXTURE_2D, texture3);
    _drawDeck(shader, moveMatrix);

    // 3. Dock boat (opaque — hull partially submerged)
    glBindTexture(GL_TEXTURE_2D, texture0);
    _drawDockBoat(shader, moveMatrix, time);

    // 4. Water surface LAST (semi-transparent, so submerged hull shows)
    _drawPondWater(shader, moveMatrix, time);
}
