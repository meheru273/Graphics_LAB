// shops2.h — Row of 3 shops (right side of fair).
// Shops are arranged along Z in local space, facing -X (rotated 90 CCW).
// Place with a single moveMatrix from main.cpp.
// #included from main.cpp; inherits all globals.
#pragma once

void drawShopRow2(Shader ourShader, glm::mat4 moveMatrix, bool useTexture)
{
    glm::mat4 I = glm::mat4(1.0f);
    glm::mat4 rotCCW = glm::rotate(I, glm::radians(-90.0f), glm::vec3(0, 1, 0));
    unsigned int tex = useTexture ? texture0 : bambooTex;

    // Shop D — front (at Z=0): popcorn + slushy shop
    glm::mat4 T = glm::translate(I, glm::vec3(0.0f, 0.0f, 0.0f));
    Shop(ourShader, moveMatrix * T * rotCCW,
         glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), tex,
         itemPopcornTex, itemSlushyTex);

    // Shop E — middle (variant, at Z=-5.5): cotton candy stall (blue + pink)
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, -5.5f));
    ShopVariant(ourShader, moveMatrix * T * rotCCW,
         glm::vec4(0.60f, 0.80f, 0.40f, 1.0f), tex,
         itemCottonBlueTex, itemCottonPinkTex);

    // Shop F — back (at Z=-11.0): teddy bear + chips shop
    T = glm::translate(I, glm::vec3(0.0f, 0.0f, -11.0f));
    Shop(ourShader, moveMatrix * T * rotCCW,
         glm::vec4(0.95f, 0.90f, 0.80f, 1.0f), tex,
         itemTeddyMultiTex, itemChipsTex);
}
