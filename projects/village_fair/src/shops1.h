// shops1.h — Row of 3 shops (left side of fair).
// Shops are arranged along Z in local space, facing +X (rotated 90 CW).
// Place with a single moveMatrix from main.cpp.
// #included from main.cpp; inherits all globals.
#pragma once

void drawShopRow1(Shader ourShader, glm::mat4 moveMatrix, bool useTexture)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 rotCW = glm::rotate(I, glm::radians(90.0f), glm::vec3(0, 1, 0));
    unsigned int tex = useTexture ? texture0 : bambooTex;

    // Shop A — front (at Z=0): cotton candy shop (pink + blue)
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    Shop(ourShader, moveMatrix * T * rotCW,
         glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), tex,
         itemCottonPinkTex, itemCottonBlueTex);

    // Shop B — middle (variant, at Z=-5.5): teddy bear + popcorn stall
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, -5.5f));
    ShopVariant(ourShader, moveMatrix * T * rotCW,
         glm::vec4(0.85f, 0.55f, 0.10f, 1.0f), tex,
         itemTeddyTex, itemTeddyMultiTex);

    // Shop C — back (at Z=-11.0): slushy + chips shop
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, -11.0f));
    Shop(ourShader, moveMatrix * T * rotCW,
         glm::vec4(0.9f, 0.85f, 0.75f, 1.0f), tex,
         itemSlushyTex, itemChipsTex);
}
