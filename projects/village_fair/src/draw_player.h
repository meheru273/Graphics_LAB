// draw_player.h -- "Human Girl" character built from cubes + spheres.
// #included from main.cpp; inherits all globals (cubeVAO, sphereVAO, etc.)
//
// Two draw modes:
//   drawPlayerThirdPerson()  -- full body for chase camera
//   drawPlayerFirstPerson()  -- arms only (viewmodel) for FPV
#pragma once

#include <cmath>

// ---------------------------------------------------------------
//  Colours
// ---------------------------------------------------------------
static const glm::vec4 SKIN_COL   (1.00f, 0.82f, 0.68f, 1.0f);
static const glm::vec4 HAIR_COL   (0.12f, 0.06f, 0.03f, 1.0f);
static const glm::vec4 DRESS_COL  (0.85f, 0.18f, 0.35f, 1.0f);  // pinkish-red dress
static const glm::vec4 DRESS_COL2 (0.95f, 0.75f, 0.20f, 1.0f);  // yellow trim
static const glm::vec4 SHOE_COL   (0.20f, 0.10f, 0.05f, 1.0f);
static const glm::vec4 EYE_COL    (0.10f, 0.10f, 0.10f, 1.0f);

// ---------------------------------------------------------------
//  Helper: set material to a flat colour
// ---------------------------------------------------------------
static void _playerMat(Shader& s, glm::vec4 col, float shin = 16.0f)
{
    s.setVec4("material.ambient",  col * 0.55f);
    s.setVec4("material.diffuse",  col);
    s.setVec4("material.specular", col * 0.15f);
    s.setFloat("material.shininess", shin);
}

// ---------------------------------------------------------------
//  Helper: draw a cube (uses cubeVAO — a 0.5-unit cube)
//  pos  = bottom-left-front corner
//  size = dimensions along X, Y, Z
// ---------------------------------------------------------------
static void _playerCube(Shader& s, glm::mat4 parent,
                        glm::vec3 pos, glm::vec3 size)
{
    glm::mat4 I(1.0f);
    // cubeVAO is a unit cube from 0..0.5 in each axis, so we scale by size/0.5
    glm::mat4 M = glm::translate(I, pos)
                 * glm::scale(I, size / 0.5f);
    s.setMat4("model", parent * M);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// ---------------------------------------------------------------
//  Helper: draw a sphere
//  ctr  = centre, r = uniform radius
// ---------------------------------------------------------------
static void _playerSphere(Shader& s, glm::mat4 parent,
                          glm::vec3 ctr, float r)
{
    glm::mat4 I(1.0f);
    glm::mat4 M = glm::translate(I, ctr)
                 * glm::scale(I, glm::vec3(r));
    s.setMat4("model", parent * M);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

// ---------------------------------------------------------------
//  Full body — for chase / third-person camera
//
//  Character faces LOCAL +Z.  Total height ~1.7 units.
//  Origin at feet centre (0, 0, 0).
//
//  walkPhase: pass sin(time * walkSpeed) for leg/arm swing
// ---------------------------------------------------------------
void drawPlayerThirdPerson(Shader& s, glm::mat4 rootMatrix,
                           unsigned int plainTex, float walkPhase)
{
    glBindTexture(GL_TEXTURE_2D, plainTex);
    glm::mat4 I(1.0f);

    float armSwing = walkPhase * 0.35f;   // radians
    float legSwing = walkPhase * 0.30f;

    // ---- Legs (two cubes from Y=0 upward) ----
    float legW = 0.12f, legD = 0.14f, legH = 0.55f;
    float hipY = 0.0f;

    // Left leg
    _playerMat(s, SKIN_COL);
    {
        glm::mat4 pivot = rootMatrix
            * glm::translate(I, glm::vec3(-0.07f, hipY + legH, 0.0f))
            * glm::rotate(I, legSwing, glm::vec3(1, 0, 0))
            * glm::translate(I, glm::vec3(0.0f, -legH, 0.0f));
        _playerCube(s, pivot, glm::vec3(-legW*0.5f, 0.0f, -legD*0.5f), glm::vec3(legW, legH, legD));
        // shoe
        _playerMat(s, SHOE_COL);
        _playerCube(s, pivot, glm::vec3(-legW*0.5f - 0.01f, -0.06f, -legD*0.5f - 0.03f),
                    glm::vec3(legW + 0.02f, 0.08f, legD + 0.06f));
    }
    // Right leg
    _playerMat(s, SKIN_COL);
    {
        glm::mat4 pivot = rootMatrix
            * glm::translate(I, glm::vec3(0.07f, hipY + legH, 0.0f))
            * glm::rotate(I, -legSwing, glm::vec3(1, 0, 0))
            * glm::translate(I, glm::vec3(0.0f, -legH, 0.0f));
        _playerCube(s, pivot, glm::vec3(-legW*0.5f, 0.0f, -legD*0.5f), glm::vec3(legW, legH, legD));
        _playerMat(s, SHOE_COL);
        _playerCube(s, pivot, glm::vec3(-legW*0.5f - 0.01f, -0.06f, -legD*0.5f - 0.03f),
                    glm::vec3(legW + 0.02f, 0.08f, legD + 0.06f));
    }

    // ---- Skirt / Dress lower ----
    float skirtY = legH - 0.05f;
    float skirtW = 0.34f, skirtH = 0.30f, skirtD = 0.24f;
    _playerMat(s, DRESS_COL);
    _playerCube(s, rootMatrix, glm::vec3(-skirtW*0.5f, skirtY, -skirtD*0.5f),
                glm::vec3(skirtW, skirtH, skirtD));
    // Trim
    _playerMat(s, DRESS_COL2);
    _playerCube(s, rootMatrix, glm::vec3(-skirtW*0.5f - 0.005f, skirtY, -skirtD*0.5f - 0.005f),
                glm::vec3(skirtW + 0.01f, 0.04f, skirtD + 0.01f));

    // ---- Torso ----
    float torsoY = skirtY + skirtH;
    float torsoW = 0.30f, torsoH = 0.38f, torsoD = 0.18f;
    _playerMat(s, DRESS_COL);
    _playerCube(s, rootMatrix, glm::vec3(-torsoW*0.5f, torsoY, -torsoD*0.5f),
                glm::vec3(torsoW, torsoH, torsoD));

    // ---- Arms ----
    float armW = 0.09f, armH = 0.42f, armD = 0.09f;
    float shoulderY = torsoY + torsoH - 0.04f;

    // Left arm
    _playerMat(s, SKIN_COL);
    {
        glm::mat4 pivot = rootMatrix
            * glm::translate(I, glm::vec3(-torsoW*0.5f - armW*0.5f, shoulderY, 0.0f))
            * glm::rotate(I, -armSwing, glm::vec3(1, 0, 0))
            * glm::translate(I, glm::vec3(0.0f, -armH, 0.0f));
        _playerCube(s, pivot, glm::vec3(-armW*0.5f, 0.0f, -armD*0.5f), glm::vec3(armW, armH, armD));
        // Hand
        _playerSphere(s, pivot, glm::vec3(0.0f, -0.02f, 0.0f), 0.045f);
    }
    // Right arm
    {
        glm::mat4 pivot = rootMatrix
            * glm::translate(I, glm::vec3(torsoW*0.5f + armW*0.5f, shoulderY, 0.0f))
            * glm::rotate(I, armSwing, glm::vec3(1, 0, 0))
            * glm::translate(I, glm::vec3(0.0f, -armH, 0.0f));
        _playerCube(s, pivot, glm::vec3(-armW*0.5f, 0.0f, -armD*0.5f), glm::vec3(armW, armH, armD));
        _playerSphere(s, pivot, glm::vec3(0.0f, -0.02f, 0.0f), 0.045f);
    }

    // ---- Neck ----
    float neckY = torsoY + torsoH;
    _playerMat(s, SKIN_COL);
    _playerCube(s, rootMatrix, glm::vec3(-0.04f, neckY, -0.04f), glm::vec3(0.08f, 0.06f, 0.08f));

    // ---- Head ----
    float headY = neckY + 0.06f;
    float headR = 0.14f;
    _playerMat(s, SKIN_COL, 24.0f);
    _playerSphere(s, rootMatrix, glm::vec3(0.0f, headY + headR, 0.0f), headR);

    // ---- Hair ----
    _playerMat(s, HAIR_COL, 32.0f);
    // Top hair cap
    _playerSphere(s, rootMatrix, glm::vec3(0.0f, headY + headR + 0.02f, -0.02f), headR + 0.02f);
    // Back hair (long)
    _playerCube(s, rootMatrix, glm::vec3(-0.11f, headY - 0.10f, -0.10f),
                glm::vec3(0.22f, 0.32f, 0.08f));
    // Side strands
    _playerCube(s, rootMatrix, glm::vec3(-0.14f, headY + 0.02f, -0.06f), glm::vec3(0.04f, 0.22f, 0.06f));
    _playerCube(s, rootMatrix, glm::vec3( 0.10f, headY + 0.02f, -0.06f), glm::vec3(0.04f, 0.22f, 0.06f));

    // ---- Eyes (small dark spheres) ----
    _playerMat(s, EYE_COL);
    _playerSphere(s, rootMatrix, glm::vec3(-0.05f, headY + headR + 0.02f, 0.12f), 0.025f);
    _playerSphere(s, rootMatrix, glm::vec3( 0.05f, headY + headR + 0.02f, 0.12f), 0.025f);

    // ---- Hair ribbon / bow ----
    _playerMat(s, DRESS_COL2);
    _playerCube(s, rootMatrix, glm::vec3(-0.06f, headY + headR*2 + 0.02f, -0.03f),
                glm::vec3(0.12f, 0.04f, 0.06f));
}

// ---------------------------------------------------------------
//  First-person viewmodel — arms only, positioned in screen space
//
//  viewmodelMatrix should be the inverse-view so arms stay fixed
//  relative to the camera.  walkPhase for bobbing arms.
// ---------------------------------------------------------------
void drawPlayerFirstPerson(Shader& s, glm::mat4 viewmodelMatrix,
                           unsigned int plainTex, float walkPhase)
{
    glBindTexture(GL_TEXTURE_2D, plainTex);
    glm::mat4 I(1.0f);

    float armBob = walkPhase * 0.06f;

    // Position arms in front-bottom of camera view
    // Left arm — bottom-left of screen
    _playerMat(s, SKIN_COL);
    {
        glm::mat4 armBase = viewmodelMatrix
            * glm::translate(I, glm::vec3(-0.28f, -0.35f + armBob, -0.45f))
            * glm::rotate(I, glm::radians(-75.0f), glm::vec3(1, 0, 0))
            * glm::rotate(I, glm::radians(15.0f), glm::vec3(0, 0, 1));
        _playerCube(s, armBase, glm::vec3(-0.045f, 0.0f, -0.045f), glm::vec3(0.09f, 0.38f, 0.09f));
        // Sleeve
        _playerMat(s, DRESS_COL);
        _playerCube(s, armBase, glm::vec3(-0.055f, 0.20f, -0.055f), glm::vec3(0.11f, 0.20f, 0.11f));
        // Hand
        _playerMat(s, SKIN_COL);
        _playerSphere(s, armBase, glm::vec3(0.0f, -0.02f, 0.0f), 0.048f);
    }

    // Right arm — bottom-right of screen
    _playerMat(s, SKIN_COL);
    {
        glm::mat4 armBase = viewmodelMatrix
            * glm::translate(I, glm::vec3(0.28f, -0.35f - armBob, -0.45f))
            * glm::rotate(I, glm::radians(-75.0f), glm::vec3(1, 0, 0))
            * glm::rotate(I, glm::radians(-15.0f), glm::vec3(0, 0, 1));
        _playerCube(s, armBase, glm::vec3(-0.045f, 0.0f, -0.045f), glm::vec3(0.09f, 0.38f, 0.09f));
        _playerMat(s, DRESS_COL);
        _playerCube(s, armBase, glm::vec3(-0.055f, 0.20f, -0.055f), glm::vec3(0.11f, 0.20f, 0.11f));
        _playerMat(s, SKIN_COL);
        _playerSphere(s, armBase, glm::vec3(0.0f, -0.02f, 0.0f), 0.048f);
    }
}
