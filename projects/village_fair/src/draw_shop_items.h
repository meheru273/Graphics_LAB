// draw_shop_items.h — Billboard-textured shop items on counters/shelves.
// #included from main.cpp; inherits all globals.
//
// Each item is a camera-facing quad (cylindrical billboarding — Y-axis only)
// with alpha-tested RGBA textures.
#pragma once

#include <cmath>

// ──────────────────────────────────────────────────────────────
//  Billboard quad VAO (created once, lazily)
// ──────────────────────────────────────────────────────────────
static unsigned int s_billboardVAO = 0;
static unsigned int s_billboardVBO = 0;

static void _ensureBillboardVAO()
{
    if (s_billboardVAO != 0) return;

    // Unit quad centred at origin, 1x1 in XY, facing +Z
    // Format: pos(3) + normal(3) + uv(2) = 8 floats per vertex
    float verts[] = {
        // Front face (normal +Z)
        -0.5f,  0.0f, 0.0f,   0,0,1,   0,0,   // bottom-left
         0.5f,  0.0f, 0.0f,   0,0,1,   1,0,   // bottom-right
         0.5f,  1.0f, 0.0f,   0,0,1,   1,1,   // top-right
        -0.5f,  0.0f, 0.0f,   0,0,1,   0,0,   // bottom-left
         0.5f,  1.0f, 0.0f,   0,0,1,   1,1,   // top-right
        -0.5f,  1.0f, 0.0f,   0,0,1,   0,1,   // top-left
        // Back face (normal -Z, so visible from behind too)
         0.5f,  0.0f, 0.0f,   0,0,-1,  0,0,
        -0.5f,  0.0f, 0.0f,   0,0,-1,  1,0,
        -0.5f,  1.0f, 0.0f,   0,0,-1,  1,1,
         0.5f,  0.0f, 0.0f,   0,0,-1,  0,0,
        -0.5f,  1.0f, 0.0f,   0,0,-1,  1,1,
         0.5f,  1.0f, 0.0f,   0,0,-1,  0,1,
    };

    glGenVertexArrays(1, &s_billboardVAO);
    glGenBuffers(1, &s_billboardVBO);
    glBindVertexArray(s_billboardVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_billboardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Draw a single billboard item
//
//  worldPos:  base centre of the item in MODEL space (pre-moveMatrix)
//  scale:     width & height of the quad
//  tex:       RGBA texture with transparent background
//  shader:    must already be active
//  moveMatrix: the parent transform (shop's moveMatrix)
// ──────────────────────────────────────────────────────────────
static void _drawBillboardItem(Shader& shader, glm::mat4 moveMatrix,
                               glm::vec3 worldPos, float scaleW, float scaleH,
                               unsigned int tex)
{
    _ensureBillboardVAO();

    // Bind texture and query its actual dimensions for correct aspect ratio
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    int texW = 1, texH = 1;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &texW);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);
    float aspect = (texH > 0) ? (float)texW / (float)texH : 1.0f;

    // Adjust scale to preserve texture aspect ratio
    // scaleH is the desired height; width is derived from aspect
    float finalW = scaleH * aspect;
    float finalH = scaleH;

    // Compute the item's world-space position
    glm::vec4 wp4 = moveMatrix * glm::vec4(worldPos, 1.0f);
    glm::vec3 wp(wp4);

    // Cylindrical billboarding: face camera on Y-axis only
    glm::vec3 toCam = camera.Position - wp;
    toCam.y = 0.0f;
    float len = glm::length(toCam);
    float yawAngle = 0.0f;
    if (len > 0.001f)
        yawAngle = atan2f(toCam.x, toCam.z);

    glm::mat4 I(1.0f);
    glm::mat4 M = glm::translate(I, wp)
                 * glm::rotate(I, yawAngle, glm::vec3(0, 1, 0))
                 * glm::scale(I, glm::vec3(finalW, finalH, 1.0f));

    // Enable alpha test in shader
    shader.setBool("alphaTest", true);

    // Bright white material — let the texture colour show through fully
    shader.setBool("lightingOn", false);
    glm::vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
    shader.setVec4("material.ambient",  white);
    shader.setVec4("material.diffuse",  white);
    shader.setVec4("material.specular", glm::vec4(0.0f));
    shader.setFloat("material.shininess", 1.0f);

    shader.setMat4("model", M);
    glBindVertexArray(s_billboardVAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);

    shader.setBool("lightingOn", lightingOn);
    shader.setBool("alphaTest", false);
}

// ──────────────────────────────────────────────────────────────
//  Draw a contact shadow beneath an item
// ──────────────────────────────────────────────────────────────
static void _drawContactShadow(Shader& shader, glm::mat4 moveMatrix,
                                glm::vec3 worldPos, float radius)
{
    glm::vec4 wp4 = moveMatrix * glm::vec4(worldPos, 1.0f);
    glm::vec3 wp(wp4);

    glm::mat4 I(1.0f);
    // Flat sphere on the surface (Y slightly above to avoid z-fighting)
    glm::mat4 M = glm::translate(I, glm::vec3(wp.x, wp.y + 0.003f, wp.z))
                 * glm::scale(I, glm::vec3(radius, 0.005f, radius));

    glm::vec4 shadowCol(0.0f, 0.0f, 0.0f, 0.3f);
    shader.setVec4("material.ambient",  shadowCol);
    shader.setVec4("material.diffuse",  shadowCol);
    shader.setVec4("material.specular", glm::vec4(0.0f));
    shader.setFloat("material.shininess", 1.0f);
    shader.setBool("lightingOn", false);

    shader.setMat4("model", M);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    shader.setBool("lightingOn", lightingOn);
}

// ──────────────────────────────────────────────────────────────
//  Draw a set of billboard items on a shop counter or shelf
//
//  positions[]: array of local-space positions (pre-moveMatrix)
//  textures[]:  array of texture IDs to cycle through
//  numItems:    how many items to place
//  numTextures: how many textures in the cycle
//  baseScale:   base size of each billboard quad
// ──────────────────────────────────────────────────────────────
static void drawShopItems(Shader& shader, glm::mat4 moveMatrix,
                          glm::vec3 positions[], int numItems,
                          unsigned int textures[], int numTextures,
                          float baseScale = 0.35f)
{
    // Simple pseudo-random scale variation per item
    for (int i = 0; i < numItems; ++i) {
        unsigned int tex = textures[i % numTextures];
        // Scale variation: 0.92 to 1.08
        float variation = 0.92f + 0.16f * ((i * 7 + 3) % 10) / 9.0f;
        float w = baseScale * variation;
        float h = baseScale * variation;

        _drawBillboardItem(shader, moveMatrix, positions[i], w, h, tex);
        _drawContactShadow(shader, moveMatrix, positions[i], w * 0.35f);
    }
}
