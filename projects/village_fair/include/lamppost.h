// Lamppost as a self-contained local-space object.
//
// Each lamppost draws BOTH its stand (base/pillar/arm cubes, using the
// regular lit shader) AND a glowing white bulb sphere at the arm tip
// (using the light-cube shader). Callers pass a world-space transform
// whose origin is the centre of the lamppost's base on the local ground.
//
// The cube geometry uses the unit cube spanning 0..0.5 on each axis
// (defined in main.cpp). After scale(s) a piece has size s/2, so pieces
// that need to be centred on local X/Z are translated by -s/4.
//
// The bulb uses the `lamp` Sphere from main.cpp and is drawn with
// `lightCubeShader`. After the call, this function leaves `ourShader`
// active and `cubeVAO` bound, so subsequent cube draws in the caller
// continue to work unchanged.
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "sphere.h"

extern unsigned int cubeVAO;

inline void drawLampPost(Shader& ourShader,
                         Shader& lightCubeShader,
                         Sphere& bulbSphere,
                         const glm::mat4& view,
                         const glm::mat4& projection,
                         const glm::mat4& worldMatrix)
{
    const glm::mat4 I(1.0f);
    glm::mat4 t, s, local;

    // ── Stand (base, pillar, arm) ───────────────────────────────────────
    ourShader.use();
    glBindVertexArray(cubeVAO);

    auto setMaterial = [&](const glm::vec4& c, const glm::vec4& spec, float sh) {
        ourShader.setVec4("material.ambient",  c);
        ourShader.setVec4("material.diffuse",  c);
        ourShader.setVec4("material.specular", spec);
        ourShader.setFloat("material.shininess", sh);
    };

    // 1. Base (dark plinth) — 0.40 × 0.16 × 0.40, sitting on local ground.
    t = glm::translate(I, glm::vec3(-0.20f, 0.0f, -0.20f));
    s = glm::scale(I, glm::vec3(0.8f, 0.32f, 0.8f));
    local = t * s;
    ourShader.setMat4("model", worldMatrix * local);
    setMaterial(glm::vec4(0.10f, 0.10f, 0.10f, 1.0f),
                glm::vec4(0.30f, 0.30f, 0.30f, 1.0f), 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 2. Pillar — 0.06 × 1.60 × 0.06, centred on X/Z, on top of base.
    //    Rises from y = 0.16 to y = 1.76.
    t = glm::translate(I, glm::vec3(-0.03f, 0.16f, -0.03f));
    s = glm::scale(I, glm::vec3(0.12f, 3.20f, 0.12f));
    local = t * s;
    ourShader.setMat4("model", worldMatrix * local);
    setMaterial(glm::vec4(0.25f, 0.30f, 0.25f, 1.0f),
                glm::vec4(0.30f, 0.40f, 0.30f, 1.0f), 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 3. Horizontal arm — 0.20 (X) × 0.04 (Y) × 0.04 (Z).
    //    Extends from pillar right edge (x = 0.03) to arm tip (x = 0.23),
    //    top flush under pillar top (y = 1.74), centred on Z.
    t = glm::translate(I, glm::vec3(0.03f, 1.70f, -0.02f));
    s = glm::scale(I, glm::vec3(0.40f, 0.08f, 0.08f));
    local = t * s;
    ourShader.setMat4("model", worldMatrix * local);
    setMaterial(glm::vec4(0.20f, 0.20f, 0.20f, 1.0f),
                glm::vec4(0.50f, 0.50f, 0.50f, 1.0f), 64.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // ── Glowing bulb sphere ─────────────────────────────────────────────
    // Drawn with lightCubeShader in a separate pass. The Sphere class is
    // a unit sphere (radius 1), so a scale of 0.06 gives a bulb radius of
    // 0.06. Centre it at the arm tip (x = 0.23), just under the arm's
    // bottom (y = 1.66 so the top of the sphere at y = 1.72 touches the
    // arm at y ≈ 1.70), on the Z centreline.
    {
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec4("bodyColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glm::mat4 bulbLocal =
            glm::translate(I, glm::vec3(0.23f, 1.66f, 0.0f)) *
            glm::scale(I, glm::vec3(0.06f, 0.06f, 0.06f));
        lightCubeShader.setMat4("model", worldMatrix * bulbLocal);

        bulbSphere.drawSphere(lightCubeShader);
    }

    // ── Restore state for subsequent caller draws ──────────────────────
    ourShader.use();
    glBindVertexArray(cubeVAO);
}
