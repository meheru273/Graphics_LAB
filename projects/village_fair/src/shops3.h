// shops3.h — Row of 3 shops (back wall of fair).
// Shops are arranged along X in local space, facing +Z (no rotation / default).
// Place with a single moveMatrix from main.cpp.
// #included from main.cpp; inherits all globals.
#pragma once

void drawShopRow3(Shader ourShader, glm::mat4 moveMatrix, bool useTexture)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 rot180 = glm::rotate(I, glm::radians(180.0f), glm::vec3(0, 1, 0));
    unsigned int tex = useTexture ? texture0 : bambooTex;

    // Shop G — left (at X=0): chips + popcorn stall
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    ShopVariant(ourShader, moveMatrix * T * rot180,
         glm::vec4(0.70f, 0.30f, 0.20f, 1.0f), tex,
         itemChipsTex, itemPopcornTex);

    // Shop H — center (at X=6.0): slushy + cotton candy pink
    // Shop() faces +Z by default; no rot180 needed here (ShopVariant is opposite)
    T = glm::translate(I, glm::vec3(6.0f, 0.0f, 0.0f));
    Shop(ourShader, moveMatrix * T,
         glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), tex,
         itemSlushyTex, itemCottonPinkTex);

    // Shop I — right (at X=12.0): teddy bear stall
    T = glm::translate(I, glm::vec3(12.0f, 0.0f, 0.0f));
    ShopVariant(ourShader, moveMatrix * T * rot180,
         glm::vec4(0.80f, 0.65f, 0.30f, 1.0f), tex,
         itemTeddyTex, itemTeddyMultiTex);
}
