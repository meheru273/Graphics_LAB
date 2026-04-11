#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "shader.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Cylinder VAO
//  Unit cylinder: radius=1, height=1, bottom at Y=0, top at Y=1, axis = Y.
//  Vertex layout: pos(3) + normal(3) + uv(2) = 8 floats  (matches cubeVAO)
// ─────────────────────────────────────────────────────────────────────────────
static unsigned int banyan_makeCylinderVAO(int segs, unsigned int& vbo, unsigned int& ebo, int& idxCount)
{
    const float TWO_PI = 6.28318530f;
    std::vector<float>        verts;
    std::vector<unsigned int> inds;

    // SIDE: two rings, (segs+1) verts each so UVs wrap cleanly
    for (int i = 0; i <= segs; ++i) {
        float a  = TWO_PI * i / segs;
        float cx = cosf(a), cz = sinf(a);
        float u  = (float)i / segs;
        // bottom
        verts.insert(verts.end(), { cx, 0.0f, cz,  cx, 0.0f, cz,  u, 0.0f });
        // top
        verts.insert(verts.end(), { cx, 1.0f, cz,  cx, 0.0f, cz,  u, 1.0f });
    }
    for (int i = 0; i < segs; ++i) {
        unsigned int b0=i*2, t0=i*2+1, b1=(i+1)*2, t1=(i+1)*2+1;
        inds.insert(inds.end(), { b0, b1, t1,  b0, t1, t0 });
    }

    // BOTTOM CAP  (normal 0,-1,0)
    int cBot = (int)verts.size() / 8;
    verts.insert(verts.end(), { 0,0,0, 0,-1,0, 0.5f,0.5f });
    for (int i = 0; i < segs; ++i) {
        float a = TWO_PI * i / segs;
        float cx = cosf(a), cz = sinf(a);
        verts.insert(verts.end(), { cx, 0, cz, 0,-1,0, cx*0.5f+0.5f, cz*0.5f+0.5f });
    }
    for (int i = 0; i < segs; ++i)
        inds.insert(inds.end(), { (unsigned)cBot,
                                   (unsigned)(cBot+1+((i+1)%segs)),
                                   (unsigned)(cBot+1+i) });

    // TOP CAP  (normal 0,+1,0)
    int cTop = (int)verts.size() / 8;
    verts.insert(verts.end(), { 0,1,0, 0,1,0, 0.5f,0.5f });
    for (int i = 0; i < segs; ++i) {
        float a = TWO_PI * i / segs;
        float cx = cosf(a), cz = sinf(a);
        verts.insert(verts.end(), { cx, 1, cz, 0,1,0, cx*0.5f+0.5f, cz*0.5f+0.5f });
    }
    for (int i = 0; i < segs; ++i)
        inds.insert(inds.end(), { (unsigned)cTop,
                                   (unsigned)(cTop+1+i),
                                   (unsigned)(cTop+1+((i+1)%segs)) });

    idxCount = (int)inds.size();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size()*sizeof(unsigned int), inds.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return VAO;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Leaf cross VAO
//  Two perpendicular quads ("+" shape), each 1×1, bottom at Y=0, centred on XZ.
//  Both faces of each quad are rendered so the leaf is visible from all sides.
// ─────────────────────────────────────────────────────────────────────────────
static unsigned int banyan_makeLeafVAO(unsigned int& vbo, unsigned int& ebo)
{
    float v[] = {
        // Quad A – XY plane, front (+Z normal)
        -0.5f,0,0,  0,0,1,  0,0,
         0.5f,0,0,  0,0,1,  1,0,
         0.5f,1,0,  0,0,1,  1,1,
        -0.5f,1,0,  0,0,1,  0,1,
        // Quad A – back  (-Z normal)
        -0.5f,0,0,  0,0,-1, 1,0,
         0.5f,0,0,  0,0,-1, 0,0,
         0.5f,1,0,  0,0,-1, 0,1,
        -0.5f,1,0,  0,0,-1, 1,1,
        // Quad B – YZ plane, front (+X normal)
         0,0,-0.5f, 1,0,0,  0,0,
         0,0, 0.5f, 1,0,0,  1,0,
         0,1, 0.5f, 1,0,0,  1,1,
         0,1,-0.5f, 1,0,0,  0,1,
        // Quad B – back  (-X normal)
         0,0,-0.5f,-1,0,0,  1,0,
         0,0, 0.5f,-1,0,0,  0,0,
         0,1, 0.5f,-1,0,0,  0,1,
         0,1,-0.5f,-1,0,0,  1,1,
    };
    unsigned int idx[] = {
        0,1,2, 0,2,3,         // A front
        4,6,5, 4,7,6,         // A back
        8,9,10, 8,10,11,      // B front
        12,14,13, 12,15,14    // B back
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return VAO;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Fractal branch baker
//  base   – transform to the BASE of this branch (local Y+ runs along branch)
//  depth  – 5 = trunk … 0 = tips (where leaves spawn)
//  len    – branch length in world units
//  radius – branch radius
//  seed   – deterministic random seed for this branch
// ─────────────────────────────────────────────────────────────────────────────
static void banyan_bakeBranch(
    std::vector<glm::mat4>& branches,
    std::vector<glm::mat4>& leaves,
    const glm::mat4& base,
    int   depth,
    float len,
    float radius,
    unsigned int seed)
{
    const float TWO_PI_B = 6.28318530f;

    // Record cylinder transform: scale unit-cylinder to (radius, len, radius)
    branches.push_back(base * glm::scale(glm::mat4(1.0f), glm::vec3(radius, len, radius)));

    // Move to the tip of this branch
    glm::mat4 tip = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, len, 0.0f));

    if (depth == 0) {
        // Leaf cluster – 12 leaves per tip, deterministic random jitter
        for (int k = 0; k < 12; ++k) {
            unsigned int r = seed * 1664525u + (unsigned)k * 22695477u + 1013904223u;
            float jx  = ((r & 0xFF) / 255.0f - 0.5f) * 1.4f;
            float jy  = (((r>>8) & 0xFF) / 255.0f)   * 0.9f;
            float jz  = (((r>>16)& 0xFF) / 255.0f - 0.5f) * 1.4f;
            float yaw = ((r>>24)& 0xFF) / 255.0f * TWO_PI_B;
            float sc  = 0.30f + (((r>>4) & 0xFF) / 255.0f) * 0.30f;
            leaves.push_back(
                tip *
                glm::translate(glm::mat4(1.0f), glm::vec3(jx, jy, jz)) *
                glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(sc, sc, sc))
            );
        }
        return;
    }

    // Number of children: trunk level gets 4 for better spread, rest 3
    int  nChildren  = (depth == 5) ? 4 : 3;
    // Outward tilt increases toward tips for a natural droop/spread
    float tilt      = 22.0f + (5 - depth) * 7.0f;
    float childLen  = len    * 0.63f;
    float childRad  = radius * 0.60f;

    for (int i = 0; i < nChildren; ++i) {
        unsigned int r = seed * 1664525u + (unsigned)i * 22695477u + 1013904223u;
        float yaw   = TWO_PI_B * i / nChildren;
        // ±8° random twist so branches don't look perfectly symmetric
        float twist = (((r>>8)&0xFF)/255.0f - 0.5f) * 16.0f;
        float pitch = tilt + (((r>>16)&0xFF)/255.0f - 0.5f) * 10.0f;

        glm::mat4 childBase =
            tip *
            glm::rotate(glm::mat4(1.0f), yaw + glm::radians(twist), glm::vec3(0,1,0)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(pitch),       glm::vec3(0,0,1));

        banyan_bakeBranch(branches, leaves, childBase, depth-1, childLen, childRad, r);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  BanyanTree — create once, draw every frame
// ─────────────────────────────────────────────────────────────────────────────
struct BanyanTree {
    unsigned int cylVAO = 0, cylVBO = 0, cylEBO = 0;
    unsigned int leafVAO= 0, leafVBO= 0, leafEBO= 0;
    int cylIndexCount   = 0;
    std::vector<glm::mat4> branches;
    std::vector<glm::mat4> leaves;

    void build() {
        cylVAO  = banyan_makeCylinderVAO(14, cylVBO, cylEBO, cylIndexCount);
        leafVAO = banyan_makeLeafVAO(leafVBO, leafEBO);

        // Trunk: radius 0.40, height 1.9, fractal depth 5 (shortened)
        banyan_bakeBranch(branches, leaves,
            glm::mat4(1.0f), 5, 1.9f, 0.40f, 7u);
    }

    // Call once per frame inside the render loop.
    // modelMatrix – positions/scales the whole tree in world space.
    // barkTex     – GL texture id for bark (bark.jpg)
    // leafTex     – GL texture id for leaves (leaf.png, has transparency)
    void draw(Shader& shader, const glm::mat4& modelMatrix,
              unsigned int barkTex, unsigned int leafTex) const
    {
        // ── BRANCHES ───────────────────────────────────────────────────────
        glBindTexture(GL_TEXTURE_2D, barkTex);
        shader.setBool("alphaTest", false);
        shader.setVec4("material.ambient",  glm::vec4(0.35f, 0.22f, 0.12f, 1.0f));
        shader.setVec4("material.diffuse",  glm::vec4(0.45f, 0.28f, 0.14f, 1.0f));
        shader.setVec4("material.specular", glm::vec4(0.04f, 0.03f, 0.02f, 1.0f));
        shader.setFloat("material.shininess", 6.0f);

        glBindVertexArray(cylVAO);
        for (const auto& t : branches) {
            shader.setMat4("model", modelMatrix * t);
            glDrawElements(GL_TRIANGLES, cylIndexCount, GL_UNSIGNED_INT, 0);
        }

        // ── LEAVES ─────────────────────────────────────────────────────────
        // Enable blending + alpha-test so transparent pixels are cut away
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Sort leaves back-to-front would be ideal but is expensive;
        // alpha-test (discard) is good enough for foliage.
        shader.setBool("alphaTest", true);
        glBindTexture(GL_TEXTURE_2D, leafTex);
        shader.setVec4("material.ambient",  glm::vec4(0.25f, 0.55f, 0.18f, 1.0f));
        shader.setVec4("material.diffuse",  glm::vec4(0.30f, 0.65f, 0.20f, 1.0f));
        shader.setVec4("material.specular", glm::vec4(0.02f, 0.06f, 0.02f, 1.0f));
        shader.setFloat("material.shininess", 4.0f);

        glBindVertexArray(leafVAO);
        for (const auto& t : leaves) {
            shader.setMat4("model", modelMatrix * t);
            glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
        }

        shader.setBool("alphaTest", false);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }

    // Draw a flat brick ring/disk around the tree base.
    // worldPos   – same XZ as the tree, Y = ground level
    // worldScale – same scale as the tree (so platform radius matches trunk)
    // brickTex   – brickwall_2.jpg texture id
    void drawPlatform(Shader& shader, const glm::vec3& worldPos, float worldScale,
                      unsigned int brickTex) const
    {
        // Disk: acts as a seating area around the tree base.
        // ~1.2 world units wide, ~0.55 world units tall (seat height) regardless of tree scale.
        float diskR = 2.8f / worldScale;     // ~1.2 world units wide regardless of tree scale
        float diskH = 0.55f / worldScale;    // ~0.55 world units tall (seat height)

        glm::mat4 m =
            glm::translate(glm::mat4(1.0f), worldPos) *       // ground level
            glm::scale(glm::mat4(1.0f), glm::vec3(worldScale)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(diskR, diskH, diskR));

        glBindTexture(GL_TEXTURE_2D, brickTex);
        shader.setBool("alphaTest", false);
        shader.setVec4("material.ambient",  glm::vec4(0.55f, 0.42f, 0.28f, 1.0f));
        shader.setVec4("material.diffuse",  glm::vec4(0.65f, 0.50f, 0.32f, 1.0f));
        shader.setVec4("material.specular", glm::vec4(0.10f, 0.08f, 0.05f, 1.0f));
        shader.setFloat("material.shininess", 16.0f);
        shader.setMat4("model", m);

        glBindVertexArray(cylVAO);
        glDrawElements(GL_TRIANGLES, cylIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};
