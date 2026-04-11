// draw_umbrella.h — Simple beach/market umbrella.
// #included from main.cpp; inherits all globals.
#pragma once

void drawUmbrella(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 T, S, R, model;

    // Pole (thin cylinder using cubeVAO)
    glm::vec4 poleColor(0.6f, 0.55f, 0.50f, 1.0f);
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    S = glm::scale(I, glm::vec3(0.08f, 3.5f, 0.08f));
    model = T * S;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient",  poleColor * 0.5f);
    ourShader.setVec4("material.diffuse",  poleColor);
    ourShader.setVec4("material.specular", glm::vec4(0.3f));
    ourShader.setFloat("material.shininess", 64.0f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Canopy — 4 angled panels forming a pyramid/umbrella shape
    glm::vec4 canopyColor(0.85f, 0.20f, 0.15f, 1.0f); // red
    float canopyY  = 1.55f;
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

    glBindVertexArray(0);
}
