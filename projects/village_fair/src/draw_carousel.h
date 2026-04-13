// Carousel component — upscaled detailed merry-go-round.
// #included by main.cpp; inherits all globals (cubeVAO, sphereVAO, etc.)
#pragma once

#include <cmath>

// ──────────────────────────────────────────────────────────────
//  Helper: set material quickly
// ──────────────────────────────────────────────────────────────
static void _carouselMat(Shader& s, glm::vec4 col, float shin = 32.0f)
{
    s.setVec4("material.ambient",  col * 0.50f);
    s.setVec4("material.diffuse",  col);
    s.setVec4("material.specular", col * 0.20f);
    s.setFloat("material.shininess", shin);
}

// ──────────────────────────────────────────────────────────────
//  Helper: draw cube at position with size (cubeVAO is 0..0.5)
// ──────────────────────────────────────────────────────────────
static void _carouselCube(Shader& s, glm::mat4 parent,
                          glm::vec3 pos, glm::vec3 size)
{
    glm::mat4 I(1.0f);
    glm::mat4 M = glm::translate(I, pos)
                 * glm::scale(I, size / 0.5f);
    s.setMat4("model", parent * M);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// ──────────────────────────────────────────────────────────────
//  Helper: draw sphere at centre with radius
// ──────────────────────────────────────────────────────────────
static void _carouselSphere(Shader& s, glm::mat4 parent,
                            glm::vec3 ctr, float r)
{
    glm::mat4 I(1.0f);
    glm::mat4 M = glm::translate(I, ctr) * glm::scale(I, glm::vec3(r));
    s.setMat4("model", parent * M);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

// ──────────────────────────────────────────────────────────────
//  Draw a single horse from cubes + spheres
//  Origin at base of hooves, facing +Z
// ──────────────────────────────────────────────────────────────
static void _drawCarouselHorse(Shader& s, glm::mat4 root, glm::vec4 bodyCol)
{
    glm::mat4 I(1.0f);

    // Body
    _carouselMat(s, bodyCol, 24.0f);
    _carouselCube(s, root, glm::vec3(-0.18f, 0.45f, -0.15f), glm::vec3(0.36f, 0.30f, 0.55f));

    // Legs (4)
    glm::vec4 legCol = bodyCol * 0.85f; legCol.w = 1.0f;
    _carouselMat(s, legCol);
    _carouselCube(s, root, glm::vec3(-0.16f, 0.0f, -0.10f), glm::vec3(0.10f, 0.45f, 0.10f));   // FL
    _carouselCube(s, root, glm::vec3( 0.06f, 0.0f, -0.10f), glm::vec3(0.10f, 0.45f, 0.10f));   // FR
    _carouselCube(s, root, glm::vec3(-0.16f, 0.0f,  0.28f), glm::vec3(0.10f, 0.45f, 0.10f));   // BL
    _carouselCube(s, root, glm::vec3( 0.06f, 0.0f,  0.28f), glm::vec3(0.10f, 0.45f, 0.10f));   // BR

    // Neck
    _carouselMat(s, bodyCol);
    glm::mat4 neckM = root
        * glm::translate(I, glm::vec3(-0.06f, 0.75f, -0.12f))
        * glm::rotate(I, glm::radians(25.0f), glm::vec3(1, 0, 0));
    _carouselCube(s, neckM, glm::vec3(-0.06f, 0.0f, -0.06f), glm::vec3(0.12f, 0.30f, 0.12f));

    // Head
    _carouselSphere(s, root, glm::vec3(0.0f, 0.95f, -0.25f), 0.10f);

    // Ears
    _carouselCube(s, root, glm::vec3(-0.06f, 1.03f, -0.27f), glm::vec3(0.04f, 0.08f, 0.03f));
    _carouselCube(s, root, glm::vec3( 0.02f, 1.03f, -0.27f), glm::vec3(0.04f, 0.08f, 0.03f));

    // Tail
    glm::vec4 tailCol = bodyCol * 0.7f; tailCol.w = 1.0f;
    _carouselMat(s, tailCol);
    glm::mat4 tailM = root
        * glm::translate(I, glm::vec3(-0.02f, 0.65f, 0.38f))
        * glm::rotate(I, glm::radians(-30.0f), glm::vec3(1, 0, 0));
    _carouselCube(s, tailM, glm::vec3(-0.02f, 0.0f, -0.02f), glm::vec3(0.04f, 0.25f, 0.04f));

    // Saddle
    glm::vec4 saddleCol(0.75f, 0.15f, 0.15f, 1.0f);
    _carouselMat(s, saddleCol, 48.0f);
    _carouselCube(s, root, glm::vec3(-0.20f, 0.75f, -0.04f), glm::vec3(0.40f, 0.06f, 0.28f));
    // Saddle blanket
    glm::vec4 blanketCol(0.90f, 0.80f, 0.15f, 1.0f);
    _carouselMat(s, blanketCol);
    _carouselCube(s, root, glm::vec3(-0.21f, 0.74f, -0.06f), glm::vec3(0.42f, 0.03f, 0.32f));

    // Hooves (dark)
    glm::vec4 hoofCol(0.15f, 0.10f, 0.05f, 1.0f);
    _carouselMat(s, hoofCol);
    _carouselCube(s, root, glm::vec3(-0.17f, -0.04f, -0.11f), glm::vec3(0.12f, 0.06f, 0.12f));
    _carouselCube(s, root, glm::vec3( 0.05f, -0.04f, -0.11f), glm::vec3(0.12f, 0.06f, 0.12f));
    _carouselCube(s, root, glm::vec3(-0.17f, -0.04f,  0.27f), glm::vec3(0.12f, 0.06f, 0.12f));
    _carouselCube(s, root, glm::vec3( 0.05f, -0.04f,  0.27f), glm::vec3(0.12f, 0.06f, 0.12f));
}

// ──────────────────────────────────────────────────────────────
//  PUBLIC: Full carousel drawing
//  centerPos: world-space XZ centre of the ride, Y = ground
// ──────────────────────────────────────────────────────────────
void Carousal(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z)
{
    glm::mat4 I(1.0f);

    // ── Carousel parameters ────────────────────────────────────
    // The old carousel used an internal pivot at (14, 1.2, -4.25).
    // We keep the same origin system so the call-site offset still works.
    float cx = 14.0f, cz = -4.25f;
    float groundY = -0.42f;

    float baseRadius     = 3.2f;     // wooden deck radius
    float baseHeight     = 0.15f;
    float columnRadius   = 0.30f;
    float columnHeight   = 4.0f;
    float canopyRadius   = 2.8f;
    float canopyThick    = 0.12f;
    float canopyY        = groundY + columnHeight;
    float poleRadius     = 2.2f;     // ring where horses sit
    int   numHorses      = 6;
    float horseScale     = 1.4f;
    float spireHeight    = 1.2f;

    // ── Rotation logic (same speed system as before) ──
    if (isCarousalOn) {
        carousalSpeed += carousalIncrement;
        carousalSpeed = min(3.5f, carousalSpeed);
    } else {
        carousalSpeed -= carousalIncrement;
        carousalSpeed = max(0.02f, carousalSpeed);
    }
    if (carousalSpeed != 0.02f)
        carousalAngle += carousalSpeed;
    carousalAngle = fmod(carousalAngle, 360.0f);

    float currentTime = (float)glfwGetTime();
    glm::mat4 rotY = glm::rotate(I, glm::radians(-carousalAngle), glm::vec3(0, 1, 0));

    // Helper: centre-relative matrix with rotation
    auto atCentre = [&](glm::mat4 extraRot = glm::mat4(1.0f)) {
        return moveMatrix * glm::translate(I, glm::vec3(cx, 0, cz)) * extraRot;
    };

    // ════════════════════════════════════════════════════════════
    //  1. GROUND PLATFORM — large circular wooden deck
    // ════════════════════════════════════════════════════════════
    {
        glBindTexture(GL_TEXTURE_2D, bambooTex);
        glm::vec4 deckCol(0.55f, 0.35f, 0.18f, 1.0f);
        _carouselMat(ourShader, deckCol, 16.0f);

        // Approximate circle with 24 radial wedge-cubes
        int deckSegs = 24;
        for (int i = 0; i < deckSegs; ++i) {
            float a0 = glm::radians(360.0f * i / deckSegs);
            float a1 = glm::radians(360.0f * (i + 1) / deckSegs);
            float aMid = (a0 + a1) * 0.5f;
            float segW = 2.0f * baseRadius * sinf(3.14159f / deckSegs) * 1.05f;

            glm::mat4 M = glm::translate(I, glm::vec3(cx + baseRadius * 0.5f * cosf(aMid),
                                                       groundY, cz + baseRadius * 0.5f * sinf(aMid)))
                         * glm::rotate(I, -aMid, glm::vec3(0, 1, 0))
                         * glm::translate(I, glm::vec3(-segW * 0.5f, 0.0f, -baseHeight * 0.5f))
                         * glm::scale(I, glm::vec3(segW / 0.5f, baseHeight / 0.5f, baseRadius / 0.5f));
            ourShader.setMat4("model", moveMatrix * M);
            glBindVertexArray(cubeVAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // Deck rim (decorative ring)
        glm::vec4 rimCol(0.40f, 0.22f, 0.10f, 1.0f);
        _carouselMat(ourShader, rimCol);
        int rimSegs = 32;
        for (int i = 0; i < rimSegs; ++i) {
            float a = glm::radians(360.0f * i / rimSegs);
            float aNext = glm::radians(360.0f * (i + 1) / rimSegs);
            float aMid = (a + aNext) * 0.5f;
            float segLen = 2.0f * baseRadius * sinf(3.14159f / rimSegs) * 1.02f;

            glm::mat4 M = glm::translate(I, glm::vec3(cx + baseRadius * cosf(aMid),
                                                       groundY, cz + baseRadius * sinf(aMid)))
                         * glm::rotate(I, -aMid, glm::vec3(0, 1, 0))
                         * glm::translate(I, glm::vec3(-segLen * 0.5f, 0.0f, -0.06f))
                         * glm::scale(I, glm::vec3(segLen / 0.5f, 0.28f / 0.5f, 0.12f / 0.5f));
            ourShader.setMat4("model", moveMatrix * M);
            glBindVertexArray(cubeVAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

    // ════════════════════════════════════════════════════════════
    //  2. CENTRE COLUMN — thick decorated pillar (static)
    // ════════════════════════════════════════════════════════════
    {
        glBindTexture(GL_TEXTURE_2D, texture0);

        // Main column
        glm::vec4 colCol(0.72f, 0.58f, 0.28f, 1.0f);  // brass/gold
        _carouselMat(ourShader, colCol, 64.0f);
        _carouselCube(ourShader, moveMatrix,
                      glm::vec3(cx - columnRadius, groundY, cz - columnRadius),
                      glm::vec3(columnRadius * 2, columnHeight + 0.2f, columnRadius * 2));

        // Decorative bands at 3 heights
        glm::vec4 bandCol(0.85f, 0.70f, 0.25f, 1.0f);
        _carouselMat(ourShader, bandCol, 80.0f);
        float bandW = columnRadius * 1.2f;
        for (float h : {0.5f, 2.0f, 3.5f}) {
            _carouselCube(ourShader, moveMatrix,
                          glm::vec3(cx - bandW, groundY + h, cz - bandW),
                          glm::vec3(bandW * 2, 0.12f, bandW * 2));
        }

        // Mirror panels (4 sides) — shiny reflective surfaces
        glm::vec4 mirrorCol(0.85f, 0.88f, 0.92f, 1.0f);
        _carouselMat(ourShader, mirrorCol, 128.0f);
        float mW = columnRadius * 0.8f, mH = 1.2f, mD = 0.03f;
        float mY = groundY + 1.2f;
        // N/S/E/W faces
        _carouselCube(ourShader, moveMatrix, glm::vec3(cx - mW, mY, cz - columnRadius - mD), glm::vec3(mW * 2, mH, mD));
        _carouselCube(ourShader, moveMatrix, glm::vec3(cx - mW, mY, cz + columnRadius),     glm::vec3(mW * 2, mH, mD));
        _carouselCube(ourShader, moveMatrix, glm::vec3(cx - columnRadius - mD, mY, cz - mW), glm::vec3(mD, mH, mW * 2));
        _carouselCube(ourShader, moveMatrix, glm::vec3(cx + columnRadius, mY, cz - mW),     glm::vec3(mD, mH, mW * 2));
    }

    // ════════════════════════════════════════════════════════════
    //  3. CANOPY — rotating top structure
    // ════════════════════════════════════════════════════════════
    glm::mat4 spinBase = moveMatrix * glm::translate(I, glm::vec3(cx, 0, cz))
                       * rotY
                       * glm::translate(I, glm::vec3(-cx, 0, -cz));
    {
        glBindTexture(GL_TEXTURE_2D, texture0);

        // Main canopy disc (using bezier shape)
        glm::vec4 canopyCol(0.78f, 0.10f, 0.18f, 1.0f);  // dark red
        _carouselMat(ourShader, canopyCol, 32.0f);

        glm::mat4 canopyM = spinBase
            * glm::translate(I, glm::vec3(cx, canopyY, cz))
            * glm::scale(I, glm::vec3(2.8f, 0.8f, 2.8f));
        ourShader.setMat4("model", canopyM);
        glBindVertexArray(carousalVAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesCarousal.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Valance panels hanging from canopy edge (scalloped look)
        glBindVertexArray(cubeVAO);
        glm::vec4 valCol(0.90f, 0.82f, 0.20f, 1.0f);  // golden trim
        _carouselMat(ourShader, valCol);
        int valSegs = 24;
        for (int i = 0; i < valSegs; ++i) {
            float a = glm::radians(360.0f * i / valSegs);
            float vx = cx + canopyRadius * cosf(a);
            float vz = cz + canopyRadius * sinf(a);
            // Scallop: alternating depths
            float depth = (i % 2 == 0) ? 0.35f : 0.20f;

            glm::mat4 M = spinBase
                * glm::translate(I, glm::vec3(vx, canopyY - depth, vz))
                * glm::rotate(I, -a, glm::vec3(0, 1, 0))
                * glm::translate(I, glm::vec3(-0.22f, 0.0f, -0.03f))
                * glm::scale(I, glm::vec3(0.44f / 0.5f, depth / 0.5f, 0.06f / 0.5f));
            ourShader.setMat4("model", M);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // Underside disc (lighter colour so canopy has a bottom face)
        glm::vec4 underCol(0.92f, 0.85f, 0.72f, 1.0f);
        _carouselMat(ourShader, underCol);
        // Simple flat ring approximated with segments
        int underSegs = 16;
        for (int i = 0; i < underSegs; ++i) {
            float a = glm::radians(360.0f * i / underSegs);
            float aMid = a + glm::radians(360.0f / underSegs * 0.5f);
            float segW = 2.0f * canopyRadius * sinf(3.14159f / underSegs) * 1.05f;

            glm::mat4 M = spinBase
                * glm::translate(I, glm::vec3(cx + canopyRadius * 0.5f * cosf(aMid),
                                               canopyY - 0.02f, cz + canopyRadius * 0.5f * sinf(aMid)))
                * glm::rotate(I, -aMid, glm::vec3(0, 1, 0))
                * glm::translate(I, glm::vec3(-segW * 0.5f, 0.0f, -0.04f))
                * glm::scale(I, glm::vec3(segW / 0.5f, 0.04f / 0.5f, canopyRadius / 0.5f));
            ourShader.setMat4("model", M);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

    // ════════════════════════════════════════════════════════════
    //  4. CENTRAL SPIRE — decorative top
    // ════════════════════════════════════════════════════════════
    {
        float spireBaseY = canopyY + 0.4f;  // above canopy bezier top
        glm::vec4 spireCol(0.85f, 0.70f, 0.20f, 1.0f);
        _carouselMat(ourShader, spireCol, 64.0f);

        // Tapered column
        _carouselCube(ourShader, spinBase,
                      glm::vec3(cx - 0.12f, spireBaseY, cz - 0.12f),
                      glm::vec3(0.24f, spireHeight * 0.7f, 0.24f));

        // Narrow tip
        _carouselCube(ourShader, spinBase,
                      glm::vec3(cx - 0.06f, spireBaseY + spireHeight * 0.7f, cz - 0.06f),
                      glm::vec3(0.12f, spireHeight * 0.3f, 0.12f));

        // Ornament ball at top
        glm::vec4 ornCol(1.0f, 0.85f, 0.20f, 1.0f);
        _carouselMat(ourShader, ornCol, 96.0f);
        _carouselSphere(ourShader, spinBase,
                        glm::vec3(cx, spireBaseY + spireHeight + 0.08f, cz), 0.12f);

        // Spire base ring
        _carouselMat(ourShader, spireCol);
        _carouselCube(ourShader, spinBase,
                      glm::vec3(cx - 0.22f, spireBaseY - 0.06f, cz - 0.22f),
                      glm::vec3(0.44f, 0.08f, 0.44f));
    }

    // ════════════════════════════════════════════════════════════
    //  5. POLES + HORSES — rotating with the canopy
    // ════════════════════════════════════════════════════════════
    {
        glBindTexture(GL_TEXTURE_2D, texture0);

        glm::vec4 poleCol(0.78f, 0.65f, 0.25f, 1.0f);  // brass
        glm::vec4 horseColors[] = {
            glm::vec4(0.95f, 0.92f, 0.88f, 1.0f),  // white
            glm::vec4(0.50f, 0.30f, 0.15f, 1.0f),  // brown
            glm::vec4(0.20f, 0.18f, 0.16f, 1.0f),  // dark grey
            glm::vec4(0.85f, 0.75f, 0.55f, 1.0f),  // palomino
            glm::vec4(0.70f, 0.25f, 0.15f, 1.0f),  // chestnut
            glm::vec4(0.60f, 0.55f, 0.50f, 1.0f),  // grey
        };

        float poleH = columnHeight - 0.2f;

        for (int i = 0; i < numHorses; ++i) {
            float angle = glm::radians(360.0f * i / numHorses);
            float px = cx + poleRadius * cosf(angle);
            float pz = cz + poleRadius * sinf(angle);

            // Vertical bob: each horse slightly out of phase
            float bobPhase = carousalAngle * 0.05f + (float)i * 1.047f;  // ~60 deg offset each
            float bobY = sinf(bobPhase) * 0.18f;

            // Pole (brass rod from base to canopy)
            _carouselMat(ourShader, poleCol, 80.0f);
            float poleW = 0.06f;
            _carouselCube(ourShader, spinBase,
                          glm::vec3(px - poleW, groundY + baseHeight, pz - poleW),
                          glm::vec3(poleW * 2, poleH, poleW * 2));

            // Horse
            float horseBaseY = groundY + baseHeight + 0.6f + bobY;
            glm::mat4 horseRoot = spinBase
                * glm::translate(I, glm::vec3(px, horseBaseY, pz))
                * glm::rotate(I, -angle + glm::radians(90.0f), glm::vec3(0, 1, 0))
                * glm::scale(I, glm::vec3(horseScale));

            _drawCarouselHorse(ourShader, horseRoot, horseColors[i % 6]);
        }
    }

    // ════════════════════════════════════════════════════════════
    //  6. MARQUEE LIGHTS — canopy edge + spire
    // ════════════════════════════════════════════════════════════
    {
        glBindVertexArray(sphereVAO);
        glBindTexture(GL_TEXTURE_2D, texture0);
        ourShader.setBool("lightingOn", false);

        int lightPhase = (int)(currentTime * 4.0f);

        // Canopy edge lights
        int numEdgeLights = 32;
        for (int i = 0; i < numEdgeLights; ++i) {
            float a = glm::radians(360.0f * i / numEdgeLights);
            float lx = cx + (canopyRadius + 0.05f) * cosf(a);
            float lz = cz + (canopyRadius + 0.05f) * sinf(a);

            // Chase pattern: 3-colour cycle
            int pattern = (i + lightPhase) % 4;
            glm::vec4 bulbColors[] = {
                glm::vec4(1.0f, 0.20f, 0.20f, 1.0f),   // red
                glm::vec4(1.0f, 1.0f, 0.20f, 1.0f),     // yellow
                glm::vec4(0.20f, 1.0f, 0.30f, 1.0f),     // green
                glm::vec4(0.30f, 0.30f, 1.0f, 1.0f),     // blue
            };
            glm::vec4 dimCol(0.15f, 0.13f, 0.10f, 1.0f);
            glm::vec4 col = (pattern == 0) ? bulbColors[i % 4] : dimCol;

            ourShader.setVec4("material.ambient",  col);
            ourShader.setVec4("material.diffuse",  col);
            ourShader.setVec4("material.specular", glm::vec4(0.0f));
            ourShader.setFloat("material.shininess", 1.0f);

            glm::mat4 M = spinBase
                * glm::translate(I, glm::vec3(lx, canopyY - 0.05f, lz))
                * glm::scale(I, glm::vec3(0.06f));
            ourShader.setMat4("model", M);
            glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        }

        // Spire lights (vertical chase up the spire)
        float spireBaseY = canopyY + 0.4f;
        int numSpireLights = 6;
        for (int i = 0; i < numSpireLights; ++i) {
            float t = (float)i / (numSpireLights - 1);
            float ly = spireBaseY + t * spireHeight;
            float angle = t * 3.14159f * 4.0f + currentTime * 3.0f;
            float offX = 0.15f * cosf(angle);
            float offZ = 0.15f * sinf(angle);

            bool isOn = ((i + lightPhase) % 2 == 0);
            glm::vec4 bulb = isOn ? glm::vec4(1.0f, 0.90f, 0.40f, 1.0f)
                                  : glm::vec4(0.30f, 0.25f, 0.10f, 1.0f);

            ourShader.setVec4("material.ambient",  bulb);
            ourShader.setVec4("material.diffuse",  bulb);

            glm::mat4 M = spinBase
                * glm::translate(I, glm::vec3(cx + offX, ly, cz + offZ))
                * glm::scale(I, glm::vec3(0.05f));
            ourShader.setMat4("model", M);
            glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        }

        ourShader.setBool("lightingOn", lightingOn);
        glBindVertexArray(0);
    }

    glBindVertexArray(cubeVAO);
}
