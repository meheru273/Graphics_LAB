// draw_umbrella.h — Market umbrella with two chairs underneath.
// #included from main.cpp; inherits all globals.
#pragma once

// Helper: draw a single chair relative to umbrella origin
static void _drawChair(Shader& ourShader, glm::mat4 moveMatrix,
                       float offsetX, float offsetZ, float rotY)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 chairBase = moveMatrix
        * glm::translate(I, glm::vec3(offsetX, 0.0f, offsetZ))
        * glm::rotate(I, glm::radians(rotY), glm::vec3(0, 1, 0));

    glBindTexture(GL_TEXTURE_2D, texture7);
    glBindVertexArray(cubeVAO);

    glm::vec4 woodCol(0.55f, 0.35f, 0.18f, 1.0f);
    ourShader.setVec4("material.ambient",  woodCol * 0.45f);
    ourShader.setVec4("material.diffuse",  woodCol);
    ourShader.setVec4("material.specular", glm::vec4(0.12f));
    ourShader.setFloat("material.shininess", 16.0f);

    // Seat base (flat plank)
    glm::mat4 T = glm::translate(I, glm::vec3(-0.35f, 0.35f, -0.25f));
    glm::mat4 S = glm::scale(I, glm::vec3(1.4f, 0.12f, 1.0f));
    ourShader.setMat4("model", chairBase * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Backrest
    T = glm::translate(I, glm::vec3(-0.35f, 0.35f, -0.25f));
    S = glm::scale(I, glm::vec3(1.4f, 0.9f, 0.1f));
    ourShader.setMat4("model", chairBase * T * S);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 4 legs
    float legH = 0.7f;
    float legW = 0.06f;
    glm::vec3 legPositions[4] = {
        glm::vec3(-0.30f, 0.0f, -0.20f),
        glm::vec3( 0.30f, 0.0f, -0.20f),
        glm::vec3(-0.30f, 0.0f,  0.15f),
        glm::vec3( 0.30f, 0.0f,  0.15f),
    };
    S = glm::scale(I, glm::vec3(legW / 0.5f, legH / 0.5f, legW / 0.5f));
    for (int i = 0; i < 4; ++i) {
        T = glm::translate(I, legPositions[i]);
        ourShader.setMat4("model", chairBase * T * S);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

void drawUmbrella(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, R, model;

    // Pole (thin cylinder using cubeVAO) — taller stand
    glm::vec4 poleColor(0.6f, 0.55f, 0.50f, 1.0f);
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    S = glm::scale(I, glm::vec3(0.08f, 4.6f, 0.08f));  // increased from 3.5 to 4.6
    model = T * S;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient",  poleColor * 0.5f);
    ourShader.setVec4("material.diffuse",  poleColor);
    ourShader.setVec4("material.specular", glm::vec4(0.3f));
    ourShader.setFloat("material.shininess", 64.0f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Canopy — 4 angled panels forming a pyramid/umbrella shape (raised to match taller pole)
    glm::vec4 canopyColor(0.85f, 0.20f, 0.15f, 1.0f); // red
    float canopyY  = 2.10f;  // raised from 1.55 to 2.10
    float canopyR  = 1.2f;

    for (int i = 0; i < 4; ++i) {
        float angle = 90.0f * i + 45.0f;
        // Each panel: tilted outward from center pole
        T = glm::translate(I, glm::vec3(0.0f, canopyY, 0.0f));
        R = glm::rotate(I, glm::radians(angle), glm::vec3(0, 1, 0));
        glm::mat4 tilt = glm::rotate(I, glm::radians(25.0f), glm::vec3(1, 0, 0));
        S = glm::scale(I, glm::vec3(canopyR * 1.4f, 0.04f, canopyR));
        model = T * R * tilt * S;
        ourShader.setMat4("model", moveMatrix * model);
        // Alternate colors for panels
        glm::vec4 col = (i % 2 == 0) ? canopyColor :
            glm::vec4(0.95f, 0.85f, 0.20f, 1.0f); // yellow
        ourShader.setVec4("material.ambient",  col * 0.5f);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", glm::vec4(0.1f));
        ourShader.setFloat("material.shininess", 8.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Two chairs underneath the umbrella, facing each other
    _drawChair(ourShader, moveMatrix, -0.6f,  0.3f,   0.0f);   // chair 1 (front)
    _drawChair(ourShader, moveMatrix,  0.6f, -0.3f, 180.0f);   // chair 2 (back, facing opposite)

    glBindVertexArray(0);
}
