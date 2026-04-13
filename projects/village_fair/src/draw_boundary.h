// draw_boundary.h — Grounded fence perimeter with posts, sagging rope,
// bunting flags, entrance gate arch with "Village Fair" sign, and
// animated marquee lights.
// #included from main.cpp; inherits all globals (cubeVAO, sphereVAO, etc.)
#pragma once

#include <cmath>
#include <vector>

// ──────────────────────────────────────────────────────────────
//  Boundary rectangle (matches camera clamp in callbacks.h)
// ──────────────────────────────────────────────────────────────
static const float BND_XMIN = -20.0f, BND_XMAX = 18.0f;
static const float BND_ZMIN = -19.0f, BND_ZMAX = 25.0f;
static const float BND_Y    = -0.42f;  // ground level

// Gate location: bottom-left corner (near circus tent)
static const float GATE_CENTER_X = BND_XMIN + 3.0f;   // near X_MIN corner
static const float GATE_HALF_W   = 2.8f;               // 5.6-unit gap

// Post spacing & dimensions
static const float POST_SPACING  = 3.0f;
static const float POST_WIDTH    = 0.18f;
static const float POST_HEIGHT   = 2.2f;
static const float ROPE_Y_TOP    = POST_HEIGHT - 0.15f;  // rope attaches near top
static const float ROPE_SAG      = 0.25f;  // max sag at midpoint
static const float ROPE_THICK    = 0.025f;

// ──────────────────────────────────────────────────────────────
//  Helper: draw a single fence post
// ──────────────────────────────────────────────────────────────
static void _drawPost(Shader& shader, glm::mat4 moveMatrix,
                      float x, float z, unsigned int woodTex)
{
    glm::mat4 I = glm::mat4(1.0f);
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, woodTex);

    glm::vec4 postColor(0.42f, 0.26f, 0.10f, 1.0f);
    shader.setVec4("material.ambient",  postColor * 0.45f);
    shader.setVec4("material.diffuse",  postColor);
    shader.setVec4("material.specular", glm::vec4(0.12f, 0.08f, 0.05f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);

    // Post body
    glm::mat4 T = glm::translate(I, glm::vec3(x - POST_WIDTH*0.5f, BND_Y, z - POST_WIDTH*0.5f));
    glm::mat4 S = glm::scale(I, glm::vec3(POST_WIDTH/0.5f, POST_HEIGHT/0.5f, POST_WIDTH/0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Post cap (darker, slightly wider)
    glm::vec4 capColor = postColor * 0.7f;
    shader.setVec4("material.ambient",  capColor * 0.5f);
    shader.setVec4("material.diffuse",  capColor);
    float capW = POST_WIDTH * 1.3f;
    T = glm::translate(I, glm::vec3(x - capW*0.5f, BND_Y + POST_HEIGHT, z - capW*0.5f));
    S = glm::scale(I, glm::vec3(capW/0.5f, 0.08f/0.5f, capW/0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// ──────────────────────────────────────────────────────────────
//  Helper: draw sagging rope + bunting flags between two posts
// ──────────────────────────────────────────────────────────────
static void _drawRopeAndFlags(Shader& shader, glm::mat4 moveMatrix,
                              float x0, float z0, float x1, float z1,
                              unsigned int plainTex,
                              float ropeY = ROPE_Y_TOP)
{
    glm::mat4 I = glm::mat4(1.0f);
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, plainTex);

    float dx = x1 - x0, dz = z1 - z0;
    float segLen = sqrtf(dx*dx + dz*dz);
    float angleY = atan2f(-dz, dx);

    const int ROPE_SEGS = 8;
    glm::vec4 ropeColor(0.65f, 0.58f, 0.40f, 1.0f);
    shader.setVec4("material.ambient",  ropeColor * 0.5f);
    shader.setVec4("material.diffuse",  ropeColor);
    shader.setVec4("material.specular", glm::vec4(0.08f));
    shader.setFloat("material.shininess", 8.0f);

    // Draw rope as segments with parabolic sag
    for (int i = 0; i < ROPE_SEGS; ++i) {
        float t0 = (float)i / ROPE_SEGS;
        float t1 = (float)(i+1) / ROPE_SEGS;
        float tMid = (t0 + t1) * 0.5f;

        // Parabolic sag: max at middle (t=0.5), zero at ends
        float sag0 = ROPE_SAG * 4.0f * t0 * (1.0f - t0);
        float sag1 = ROPE_SAG * 4.0f * t1 * (1.0f - t1);
        float sagMid = (sag0 + sag1) * 0.5f;

        float sx = x0 + dx * tMid;
        float sz = z0 + dz * tMid;
        float sy = BND_Y + ropeY - sagMid;

        float pieceLen = segLen / ROPE_SEGS;

        glm::mat4 M = glm::translate(I, glm::vec3(sx, sy, sz))
                     * glm::rotate(I, angleY, glm::vec3(0,1,0))
                     * glm::translate(I, glm::vec3(-pieceLen*0.5f, -ROPE_THICK, -ROPE_THICK))
                     * glm::scale(I, glm::vec3(pieceLen/0.5f, (ROPE_THICK*2)/0.5f, (ROPE_THICK*2)/0.5f));
        shader.setMat4("model", moveMatrix * M);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Draw bunting flags hanging from rope
    static unsigned int flagVAO = 0;
    if (flagVAO == 0) {
        float fv[] = {
            -0.16f, 0.0f, 0.0f,  0.0f,0.0f,1.0f, 0.0f,1.0f,
             0.16f, 0.0f, 0.0f,  0.0f,0.0f,1.0f, 1.0f,1.0f,
             0.0f, -0.35f, 0.0f, 0.0f,0.0f,1.0f, 0.5f,0.0f,
             0.16f, 0.0f, 0.0f,  0.0f,0.0f,-1.0f, 1.0f,1.0f,
            -0.16f, 0.0f, 0.0f,  0.0f,0.0f,-1.0f, 0.0f,1.0f,
             0.0f, -0.35f, 0.0f, 0.0f,0.0f,-1.0f, 0.5f,0.0f,
        };
        unsigned int fVBO;
        glGenVertexArrays(1, &flagVAO);
        glGenBuffers(1, &fVBO);
        glBindVertexArray(flagVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fv), fv, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    glm::vec4 flagColors[7] = {
        glm::vec4(0.95f, 0.15f, 0.15f, 1.0f), // red
        glm::vec4(0.20f, 0.45f, 0.90f, 1.0f), // blue
        glm::vec4(0.95f, 0.85f, 0.05f, 1.0f), // yellow
        glm::vec4(0.15f, 0.75f, 0.30f, 1.0f), // green
        glm::vec4(0.90f, 0.30f, 0.80f, 1.0f), // pink
        glm::vec4(1.00f, 0.55f, 0.10f, 1.0f), // orange
        glm::vec4(0.50f, 0.20f, 0.80f, 1.0f), // purple
    };

    glBindVertexArray(flagVAO);
    int numFlags = (int)(segLen / 0.45f);
    for (int i = 0; i < numFlags; ++i) {
        float t = (0.2f + i * 0.45f) / segLen;
        if (t > 0.95f) break;

        float fx = x0 + dx * t;
        float fz = z0 + dz * t;
        float sag = ROPE_SAG * 4.0f * t * (1.0f - t);
        float fy = BND_Y + ropeY - sag;

        glm::vec4 col = flagColors[i % 7];
        glm::mat4 M = glm::translate(I, glm::vec3(fx, fy, fz))
                     * glm::rotate(I, angleY, glm::vec3(0,1,0))
                     * glm::rotate(I, glm::radians(12.0f), glm::vec3(1,0,0));
        shader.setMat4("model", moveMatrix * M);
        shader.setVec4("material.ambient",  col);
        shader.setVec4("material.diffuse",  col);
        shader.setVec4("material.specular", col * 0.1f);
        shader.setFloat("material.shininess", 8.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  Helper: draw one edge of fence (posts + rope + flags)
//  skipGateX0/X1: if on the gate edge, skip posts/rope in this X range
// ──────────────────────────────────────────────────────────────
static void _drawFenceEdge(Shader& shader, glm::mat4 moveMatrix,
                           float x0, float z0, float x1, float z1,
                           unsigned int woodTex, unsigned int plainTex,
                           float time = 0.0f,
                           float skipMin = 1e9f, float skipMax = -1e9f,
                           bool skipIsX = true)
{
    glm::mat4 I = glm::mat4(1.0f);
    float dx = x1 - x0, dz = z1 - z0;
    float edgeLen = sqrtf(dx*dx + dz*dz);
    float dirX = dx / edgeLen, dirZ = dz / edgeLen;

    // Place posts at regular intervals
    int numPosts = (int)(edgeLen / POST_SPACING) + 1;
    std::vector<float> postTs;  // parameter t for each post along edge

    for (int i = 0; i <= numPosts; ++i) {
        float t = (float)i / numPosts;
        float px = x0 + dx * t;
        float pz = z0 + dz * t;

        // Skip gate gap
        float checkVal = skipIsX ? px : pz;
        if (checkVal > skipMin && checkVal < skipMax) continue;

        postTs.push_back(t);
        _drawPost(shader, moveMatrix, px, pz, woodTex);
    }

    // Draw marquee lights on top of each post
    glBindVertexArray(sphereVAO);
    glBindTexture(GL_TEXTURE_2D, plainTex);
    shader.setBool("lightingOn", false);

    int phase = (int)(time * 3.0f);
    for (int i = 0; i < (int)postTs.size(); ++i) {
        float px = x0 + dx * postTs[i];
        float pz = z0 + dz * postTs[i];
        float ly = BND_Y + POST_HEIGHT + 0.15f;

        bool isOn = ((i + phase) % 2 == 0);
        glm::vec4 bulbOn(1.0f, 0.90f, 0.45f, 1.0f);
        glm::vec4 bulbOff(0.25f, 0.22f, 0.15f, 1.0f);
        glm::vec4 bulb = isOn ? bulbOn : bulbOff;

        shader.setVec4("material.ambient",  bulb);
        shader.setVec4("material.diffuse",  bulb);
        shader.setVec4("material.specular", glm::vec4(0.0f));
        shader.setFloat("material.shininess", 1.0f);

        glm::mat4 M = glm::translate(I, glm::vec3(px, ly, pz))
                     * glm::scale(I, glm::vec3(0.08f));
        shader.setMat4("model", moveMatrix * M);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    }
    shader.setBool("lightingOn", lightingOn);
    glBindVertexArray(cubeVAO);

    // Draw rope + flags between consecutive posts at three heights
    float ropeHeights[3] = { ROPE_Y_TOP, ROPE_Y_TOP * 0.62f, ROPE_Y_TOP * 0.30f };
    for (int h = 0; h < 3; ++h) {
        for (int i = 0; i + 1 < (int)postTs.size(); ++i) {
            float t0 = postTs[i], t1 = postTs[i+1];
            float px0 = x0 + dx*t0, pz0 = z0 + dz*t0;
            float px1 = x0 + dx*t1, pz1 = z0 + dz*t1;
            _drawRopeAndFlags(shader, moveMatrix, px0, pz0, px1, pz1, plainTex, ropeHeights[h]);
        }
    }
}

// ──────────────────────────────────────────────────────────────
//  Entrance Gate — two pillars + rainbow arch + sign + lights
// ──────────────────────────────────────────────────────────────
static void _drawEntranceGate(Shader& shader, glm::mat4 moveMatrix,
                              unsigned int woodTex, unsigned int signTex,
                              float time)
{
    glm::mat4 I = glm::mat4(1.0f);
    glBindVertexArray(cubeVAO);

    float gateZ = BND_ZMIN;
    float pillarX_L = GATE_CENTER_X - GATE_HALF_W;
    float pillarX_R = GATE_CENTER_X + GATE_HALF_W;
    float pillarH   = 3.2f;
    float pillarW   = 0.35f;

    // ── Two tall gate pillars ────────────────────────────────────
    glBindTexture(GL_TEXTURE_2D, woodTex);
    glm::vec4 pillarColor(0.38f, 0.22f, 0.08f, 1.0f);
    shader.setVec4("material.ambient",  pillarColor * 0.45f);
    shader.setVec4("material.diffuse",  pillarColor);
    shader.setVec4("material.specular", glm::vec4(0.15f, 0.10f, 0.06f, 1.0f));
    shader.setFloat("material.shininess", 20.0f);

    // Left pillar
    glm::mat4 T = glm::translate(I, glm::vec3(pillarX_L - pillarW*0.5f, BND_Y, gateZ - pillarW*0.5f));
    glm::mat4 S = glm::scale(I, glm::vec3(pillarW/0.5f, pillarH/0.5f, pillarW/0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Right pillar
    T = glm::translate(I, glm::vec3(pillarX_R - pillarW*0.5f, BND_Y, gateZ - pillarW*0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Pillar caps (decorative top)
    glm::vec4 capCol = pillarColor * 0.65f;
    shader.setVec4("material.ambient",  capCol * 0.5f);
    shader.setVec4("material.diffuse",  capCol);
    float capW = pillarW * 1.4f;
    S = glm::scale(I, glm::vec3(capW/0.5f, 0.12f/0.5f, capW/0.5f));
    T = glm::translate(I, glm::vec3(pillarX_L - capW*0.5f, BND_Y + pillarH, gateZ - capW*0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    T = glm::translate(I, glm::vec3(pillarX_R - capW*0.5f, BND_Y + pillarH, gateZ - capW*0.5f));
    shader.setMat4("model", moveMatrix * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── Rainbow arch (series of rotated cubes forming an arc) ────
    const int ARCH_SEGS = 16;
    float archSpan = pillarX_R - pillarX_L;
    float archRadius = archSpan * 0.5f;
    float archCenterX = GATE_CENTER_X;
    float archBaseY = BND_Y + pillarH - 0.1f;
    float archThick = 0.22f;
    float archDepth = 0.35f;

    // Rainbow colors for a festive look
    glm::vec4 archColors[7] = {
        glm::vec4(0.95f, 0.15f, 0.10f, 1.0f),
        glm::vec4(0.95f, 0.55f, 0.05f, 1.0f),
        glm::vec4(0.95f, 0.90f, 0.10f, 1.0f),
        glm::vec4(0.15f, 0.80f, 0.25f, 1.0f),
        glm::vec4(0.10f, 0.50f, 0.90f, 1.0f),
        glm::vec4(0.30f, 0.10f, 0.70f, 1.0f),
        glm::vec4(0.90f, 0.20f, 0.60f, 1.0f),
    };

    glBindTexture(GL_TEXTURE_2D, woodTex);
    for (int i = 0; i < ARCH_SEGS; ++i) {
        float a0 = 3.14159265f * i / ARCH_SEGS;
        float a1 = 3.14159265f * (i+1) / ARCH_SEGS;
        float aMid = (a0 + a1) * 0.5f;

        float cx = archCenterX - archRadius * cosf(aMid);
        float cy = archBaseY + archRadius * sinf(aMid);

        float segW = archRadius * 3.14159265f / ARCH_SEGS * 1.05f;
        float rot = aMid - 1.5707963f; // rotate segment to follow arc

        glm::vec4 col = archColors[i % 7];
        shader.setVec4("material.ambient",  col * 0.5f);
        shader.setVec4("material.diffuse",  col);
        shader.setVec4("material.specular", glm::vec4(0.15f));
        shader.setFloat("material.shininess", 24.0f);

        glm::mat4 M = glm::translate(I, glm::vec3(cx, cy, gateZ))
                     * glm::rotate(I, rot, glm::vec3(0,0,1))
                     * glm::translate(I, glm::vec3(-segW*0.5f, -archThick*0.5f, -archDepth*0.5f))
                     * glm::scale(I, glm::vec3(segW/0.5f, archThick/0.5f, archDepth/0.5f));
        shader.setMat4("model", moveMatrix * M);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // ── "Village Fair" sign board on the arch ─────────────────────
    // Use a custom quad with proper 0-1 UV mapping (cubeVAO has no UVs)
    static unsigned int signVAO = 0, signVBO = 0;
    if (signVAO == 0) {
        // Two triangles forming a quad, facing -Z (toward viewer outside)
        // Format: posX, posY, posZ, normX, normY, normZ, u, v
        float sv[] = {
            // Triangle 1
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,  // top-right
            // Triangle 2
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  // bottom-left
            1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,  // top-right
            0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,  // top-left

            // Back face (facing +Z, visible from inside)
            1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
            1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        };
        glGenVertexArrays(1, &signVAO);
        glGenBuffers(1, &signVBO);
        glBindVertexArray(signVAO);
        glBindBuffer(GL_ARRAY_BUFFER, signVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sv), sv, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    shader.setInt("ourTexture", 0);
    glBindTexture(GL_TEXTURE_2D, signTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    shader.setBool("textureOn", true);

    glm::vec4 signCol(1.0f, 1.0f, 1.0f, 1.0f);
    shader.setVec4("material.ambient",  signCol * 0.9f);
    shader.setVec4("material.diffuse",  signCol);
    shader.setVec4("material.specular", glm::vec4(0.1f));
    shader.setFloat("material.shininess", 8.0f);

    float signW = archSpan * 0.80f;
    float signH = 0.75f;
    float signY = archBaseY + archRadius * 0.45f;
    // Position the quad: translate to bottom-left corner, then scale to signW x signH
    T = glm::translate(I, glm::vec3(archCenterX - signW*0.5f, signY, gateZ - 0.03f));
    S = glm::scale(I, glm::vec3(signW, signH, 1.0f));
    shader.setMat4("model", moveMatrix * T * S);

    glBindVertexArray(signVAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);  // 2 faces x 2 triangles x 3 verts
    glBindVertexArray(0);

    // Restore texture state
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    shader.setBool("textureOn", textureOn);
    glBindVertexArray(cubeVAO);

    // ── Animated marquee lights along the arch ───────────────────
    glBindTexture(GL_TEXTURE_2D, woodTex);  // plain texture for lights
    glBindVertexArray(sphereVAO);

    float speed = 3.0f;
    int   phase = (int)(time * speed);

    for (int i = 0; i <= ARCH_SEGS; ++i) {
        float a = 3.14159265f * i / ARCH_SEGS;
        float lx = archCenterX - (archRadius + 0.2f) * cosf(a);
        float ly = archBaseY + (archRadius + 0.2f) * sinf(a);

        // Alternating blink pattern: even/odd toggle
        bool isOn = ((i + phase) % 2 == 0);
        glm::vec4 bulbOn(1.0f, 0.90f, 0.45f, 1.0f);   // warm white
        glm::vec4 bulbOff(0.25f, 0.22f, 0.15f, 1.0f);  // dim
        glm::vec4 bulb = isOn ? bulbOn : bulbOff;

        // Draw as emissive (no lighting influence)
        shader.setBool("lightingOn", false);
        shader.setVec4("material.ambient",  bulb);
        shader.setVec4("material.diffuse",  bulb);
        shader.setVec4("material.specular", glm::vec4(0.0f));
        shader.setFloat("material.shininess", 1.0f);

        glm::mat4 M = glm::translate(I, glm::vec3(lx, ly, gateZ - 0.1f))
                     * glm::scale(I, glm::vec3(0.07f));
        shader.setMat4("model", moveMatrix * M);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    }

    // Restore lighting
    shader.setBool("lightingOn", lightingOn);
    glBindVertexArray(0);
}

// ──────────────────────────────────────────────────────────────
//  PUBLIC: Draw entire fence perimeter + entrance gate
// ──────────────────────────────────────────────────────────────
void drawFenceBoundary(Shader& shader, glm::mat4 moveMatrix,
                       unsigned int woodTex, unsigned int plainTex,
                       unsigned int signTex, float time)
{
    glm::mat4 I = glm::mat4(1.0f);

    // Bottom edge (Z = BND_ZMIN, X from XMIN to XMAX) — has gate gap near left corner
    _drawFenceEdge(shader, moveMatrix,
                   BND_XMIN, BND_ZMIN, BND_XMAX, BND_ZMIN,
                   woodTex, plainTex, time,
                   GATE_CENTER_X - GATE_HALF_W - 0.3f,
                   GATE_CENTER_X + GATE_HALF_W + 0.3f,
                   true);

    // Top edge (Z = BND_ZMAX, X from XMIN to XMAX)
    _drawFenceEdge(shader, moveMatrix,
                   BND_XMIN, BND_ZMAX, BND_XMAX, BND_ZMAX,
                   woodTex, plainTex, time);

    // Left edge (X = BND_XMIN, Z from ZMAX down to ZMIN) — skip near bottom corner
    _drawFenceEdge(shader, moveMatrix,
                   BND_XMIN, BND_ZMAX, BND_XMIN, BND_ZMIN,
                   woodTex, plainTex, time,
                   BND_ZMIN - 0.5f,
                   BND_ZMIN + 2.0f,
                   false);

    // Right edge (X = BND_XMAX, Z from ZMAX down to ZMIN)
    _drawFenceEdge(shader, moveMatrix,
                   BND_XMAX, BND_ZMAX, BND_XMAX, BND_ZMIN,
                   woodTex, plainTex, time);

    // Entrance gate with arch, sign, and marquee lights
    _drawEntranceGate(shader, moveMatrix, woodTex, signTex, time);
}
