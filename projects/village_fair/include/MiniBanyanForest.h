#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include "shader.h"

// ─────────────────────────────────────────────────────────────────────────────
//  MiniBanyanForest
//  Renders 800 simplified banyan trees via instanced rendering.
//  All branch/leaf geometry is merged into single VBOs per tree, then
//  instanced with glDrawElementsInstanced across the XZ plane.
// ─────────────────────────────────────────────────────────────────────────────

class MiniBanyanForest {
    // ── Merged geometry for one mini-tree prototype ─────────────────────────
    unsigned int branchVAO = 0, branchVBO = 0, branchEBO = 0;
    unsigned int leafVAO   = 0, leafVBO   = 0, leafEBO   = 0;
    int branchIdxCount = 0;
    int leafIdxCount   = 0;

    // ── Instance buffers ────────────────────────────────────────────────────
    unsigned int branchInstanceVBO = 0;
    unsigned int leafInstanceVBO   = 0;
    int          numInstances      = 0;

    // ── Hash-based pseudo-random ────────────────────────────────────────────
    static unsigned int hashU(unsigned int x) {
        x ^= x >> 16;  x *= 0x45d9f3bu;
        x ^= x >> 16;  x *= 0x45d9f3bu;
        x ^= x >> 16;
        return x;
    }
    static float randF(unsigned int seed) {          // [0, 1)
        return (hashU(seed) & 0xFFFFFF) / float(0x1000000);
    }

    // ── Mini fractal branch baker (same logic as BanyanTree but shallower) ──
    void bakeMiniRecurse(
        std::vector<float>&        mergedBranchVerts,
        std::vector<unsigned int>& mergedBranchInds,
        std::vector<float>&        mergedLeafVerts,
        std::vector<unsigned int>& mergedLeafInds,
        const glm::mat4& base, int depth, float len, float radius, unsigned int seed)
    {
        const float TWO_PI = 6.28318530f;
        const int   SEGS   = 6;   // low-poly cylinders for performance

        // ── Emit cylinder for this branch ───────────────────────────────────
        glm::mat4 cylModel = base * glm::scale(glm::mat4(1.0f),
                                                glm::vec3(radius, len, radius));
        int baseVert = (int)mergedBranchVerts.size() / 8;

        // Side verts: two rings
        for (int i = 0; i <= SEGS; ++i) {
            float a  = TWO_PI * i / SEGS;
            float cx = cosf(a), cz = sinf(a);
            float u  = (float)i / SEGS;
            glm::vec3 bot = glm::vec3(cylModel * glm::vec4(cx, 0.0f, cz, 1.0f));
            glm::vec3 top = glm::vec3(cylModel * glm::vec4(cx, 1.0f, cz, 1.0f));
            glm::vec3 n   = glm::normalize(glm::mat3(transpose(inverse(cylModel))) *
                                            glm::vec3(cx, 0.0f, cz));
            mergedBranchVerts.insert(mergedBranchVerts.end(),
                { bot.x, bot.y, bot.z, n.x, n.y, n.z, u, 0.0f });
            mergedBranchVerts.insert(mergedBranchVerts.end(),
                { top.x, top.y, top.z, n.x, n.y, n.z, u, 1.0f });
        }
        for (int i = 0; i < SEGS; ++i) {
            unsigned int b0 = baseVert + i*2,     t0 = b0 + 1;
            unsigned int b1 = baseVert + (i+1)*2, t1 = b1 + 1;
            mergedBranchInds.insert(mergedBranchInds.end(),
                { b0, b1, t1,  b0, t1, t0 });
        }

        // Move to tip
        glm::mat4 tip = base * glm::translate(glm::mat4(1.0f),
                                               glm::vec3(0.0f, len, 0.0f));

        if (depth == 0) {
            // ── Leaf cluster at tip ─────────────────────────────────────────
            for (int k = 0; k < 6; ++k) {
                unsigned int r = seed * 1664525u + (unsigned)k * 22695477u + 1013904223u;
                float jx  = ((r & 0xFF) / 255.0f - 0.5f) * 0.6f;
                float jy  = (((r>>8) & 0xFF) / 255.0f)    * 0.4f;
                float jz  = (((r>>16)& 0xFF) / 255.0f - 0.5f) * 0.6f;
                float yaw = ((r>>24)& 0xFF) / 255.0f * TWO_PI;
                float sc  = 0.15f + (((r>>4) & 0xFF) / 255.0f) * 0.12f;

                glm::mat4 lm = tip *
                    glm::translate(glm::mat4(1.0f), glm::vec3(jx, jy, jz)) *
                    glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0)) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(sc));

                int lBase = (int)mergedLeafVerts.size() / 8;
                // Two perpendicular quads (cross billboard)
                float qv[][8] = {
                    {-0.5f,0,0, 0,0,1, 0,0}, { 0.5f,0,0, 0,0,1, 1,0},
                    { 0.5f,1,0, 0,0,1, 1,1}, {-0.5f,1,0, 0,0,1, 0,1},
                    {0,0,-0.5f, 1,0,0, 0,0}, {0,0, 0.5f, 1,0,0, 1,0},
                    {0,1, 0.5f, 1,0,0, 1,1}, {0,1,-0.5f, 1,0,0, 0,1},
                };
                for (int v = 0; v < 8; ++v) {
                    glm::vec3 p = glm::vec3(lm * glm::vec4(qv[v][0], qv[v][1], qv[v][2], 1));
                    glm::vec3 n = glm::normalize(glm::mat3(transpose(inverse(lm))) *
                                                  glm::vec3(qv[v][3], qv[v][4], qv[v][5]));
                    mergedLeafVerts.insert(mergedLeafVerts.end(),
                        { p.x, p.y, p.z, n.x, n.y, n.z, qv[v][6], qv[v][7] });
                }
                mergedLeafInds.insert(mergedLeafInds.end(), {
                    (unsigned)(lBase+0),(unsigned)(lBase+1),(unsigned)(lBase+2),
                    (unsigned)(lBase+0),(unsigned)(lBase+2),(unsigned)(lBase+3),
                    (unsigned)(lBase+4),(unsigned)(lBase+5),(unsigned)(lBase+6),
                    (unsigned)(lBase+4),(unsigned)(lBase+6),(unsigned)(lBase+7),
                });
            }
            return;
        }

        // ── Child branches ──────────────────────────────────────────────────
        int nChildren = (depth >= 3) ? 3 : 2;
        float tilt    = 25.0f + (3 - depth) * 10.0f;
        float cLen    = len    * 0.60f;
        float cRad    = radius * 0.55f;

        for (int i = 0; i < nChildren; ++i) {
            unsigned int r = seed * 1664525u + (unsigned)i * 22695477u + 1013904223u;
            float yaw   = TWO_PI * i / nChildren;
            float twist = (((r>>8)&0xFF)/255.0f - 0.5f) * 16.0f;
            float pitch = tilt + (((r>>16)&0xFF)/255.0f - 0.5f) * 10.0f;

            glm::mat4 childBase =
                tip *
                glm::rotate(glm::mat4(1.0f), yaw + glm::radians(twist), glm::vec3(0,1,0)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(pitch),       glm::vec3(0,0,1));

            bakeMiniRecurse(mergedBranchVerts, mergedBranchInds,
                            mergedLeafVerts,   mergedLeafInds,
                            childBase, depth-1, cLen, cRad, r);
        }
    }

    void uploadMergedMesh(
        const std::vector<float>& verts, const std::vector<unsigned int>& inds,
        unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, int& idxCount)
    {
        idxCount = (int)inds.size();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size()*sizeof(unsigned int), inds.data(), GL_STATIC_DRAW);

        // pos(3) + normal(3) + uv(2) = 8 floats
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void attachInstanceBuffer(unsigned int VAO, unsigned int& instanceVBO,
                              const std::vector<glm::mat4>& matrices)
    {
        glBindVertexArray(VAO);
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4),
                     matrices.data(), GL_STATIC_DRAW);

        // mat4 occupies 4 consecutive vec4 attribute slots (locations 3-6)
        std::size_t vec4Size = sizeof(glm::vec4);
        for (int i = 0; i < 4; ++i) {
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(glm::mat4), (void*)(i * vec4Size));
            glEnableVertexAttribArray(3 + i);
            glVertexAttribDivisor(3 + i, 1);  // one per instance
        }
        glBindVertexArray(0);
    }

public:
    // fairgroundMargin — extra margin beyond fairground rectangle
    // scatterRadius    — trees scatter up to this distance from origin
    void build(float fairgroundMargin = 22.0f, float scatterRadius = 200.0f,
               int   count = 800)
    {
        // ── 1. Bake one mini-tree prototype into merged vertex buffers ──────
        std::vector<float>        bVerts, lVerts;
        std::vector<unsigned int> bInds,  lInds;

        // Mini-tree: depth 3, trunk height 0.9, trunk radius 0.06
        bakeMiniRecurse(bVerts, bInds, lVerts, lInds,
                        glm::mat4(1.0f), 3, 0.9f, 0.06f, 42u);

        uploadMergedMesh(bVerts, bInds, branchVAO, branchVBO, branchEBO, branchIdxCount);
        uploadMergedMesh(lVerts, lInds, leafVAO,   leafVBO,   leafEBO,   leafIdxCount);

        // ── 2. Generate instance transforms ─────────────────────────────────
        //  Fairground rectangle (from flag bunting): X[-20,18] Z[-19,15]
        //  Add margin so trees don't touch the boundary
        const float FAIR_XMIN = -22.0f, FAIR_XMAX = 20.0f;
        const float FAIR_ZMIN = -21.0f, FAIR_ZMAX = 17.0f;

        std::vector<glm::mat4> instances;
        instances.reserve(count);

        for (int i = 0; i < count * 4 && (int)instances.size() < count; ++i) {
            float x = (randF(i * 3 + 0) * 2.0f - 1.0f) * scatterRadius;
            float z = (randF(i * 3 + 1) * 2.0f - 1.0f) * scatterRadius;

            // Only outside fairground rectangle
            if (x > FAIR_XMIN && x < FAIR_XMAX &&
                z > FAIR_ZMIN && z < FAIR_ZMAX) continue;

            // Jitter for natural scatter
            x += (randF(i * 3 + 2) * 2.0f - 1.0f) * 2.5f;
            z += (randF(i * 7 + 5) * 2.0f - 1.0f) * 2.5f;

            // Re-check after jitter
            if (x > FAIR_XMIN && x < FAIR_XMAX &&
                z > FAIR_ZMIN && z < FAIR_ZMAX) continue;

            // Distance-based scaling: further trees are slightly smaller
            float dist  = sqrtf(x*x + z*z);
            float scale = 1.8f + randF(i * 11 + 3) * 1.4f;  // base 1.8–3.2
            float distScale = glm::clamp(1.0f - (dist - 20.0f) /
                                          (scatterRadius * 1.2f), 0.5f, 1.0f);
            scale *= distScale;

            // Random Y rotation
            float rot = randF(i * 13 + 7) * 6.28318f;

            glm::mat4 m = glm::translate(glm::mat4(1.0f),
                                          glm::vec3(x, -0.45f, z));
            m = glm::rotate(m, rot, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(scale));

            instances.push_back(m);
        }

        numInstances = (int)instances.size();

        // ── 3. Upload instance matrices to both VAOs ────────────────────────
        attachInstanceBuffer(branchVAO, branchInstanceVBO, instances);
        attachInstanceBuffer(leafVAO,   leafInstanceVBO,   instances);
    }

    // Draw instanced forest.  Caller must have already set view/projection uniforms.
    void draw(Shader& shader, unsigned int barkTex, unsigned int leafTex) const
    {
        if (numInstances == 0) return;

        // Ensure we're on texture unit 0 and the sampler points to it
        glActiveTexture(GL_TEXTURE0);
        shader.setInt("ourTexture", 0);

        // ── Branches ────────────────────────────────────────────────────────
        glBindTexture(GL_TEXTURE_2D, barkTex);
        shader.setBool("alphaTest", false);
        shader.setVec4("material.ambient",  glm::vec4(0.30f, 0.18f, 0.10f, 1.0f));
        shader.setVec4("material.diffuse",  glm::vec4(0.40f, 0.24f, 0.12f, 1.0f));
        shader.setVec4("material.specular", glm::vec4(0.03f, 0.02f, 0.01f, 1.0f));
        shader.setFloat("material.shininess", 4.0f);

        glBindVertexArray(branchVAO);
        glDrawElementsInstanced(GL_TRIANGLES, branchIdxCount,
                                GL_UNSIGNED_INT, 0, numInstances);

        // ── Leaves ──────────────────────────────────────────────────────────
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader.setBool("alphaTest", true);
        glBindTexture(GL_TEXTURE_2D, leafTex);
        shader.setVec4("material.ambient",  glm::vec4(0.20f, 0.48f, 0.14f, 1.0f));
        shader.setVec4("material.diffuse",  glm::vec4(0.28f, 0.58f, 0.18f, 1.0f));
        shader.setVec4("material.specular", glm::vec4(0.02f, 0.04f, 0.02f, 1.0f));
        shader.setFloat("material.shininess", 4.0f);

        glBindVertexArray(leafVAO);
        glDrawElementsInstanced(GL_TRIANGLES, leafIdxCount,
                                GL_UNSIGNED_INT, 0, numInstances);

        shader.setBool("alphaTest", false);
        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }
};
