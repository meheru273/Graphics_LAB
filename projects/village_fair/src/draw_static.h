// This file is #included by main.cpp.
// It is part of the Village-fair project split for readability.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void load_texture(unsigned int &texture, string image_name, GLenum format)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_name.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum actualFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, actualFormat, width, height, 0, actualFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture "<<image_name << endl;
    }
    stbi_image_free(data);
}

void SetupPointLight(PointLight &pointLight, Shader ourShader, int lightNum)
{
    pointLight.setUpLight(ourShader);
    if (!pointLightOn[pointLight.lightNumber - 1])
        pointLight.turnOff();
    if (!ambientOn)
        pointLight.turnAmbientOff();
    if (!diffuseOn)
        pointLight.turnDiffuseOff();
    if (!specularOn)
        pointLight.turnSpecularOff();
}


//OBJECTS

void drawCube(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y, glm::vec4 color, float spec, float shininess)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateYMatrix *scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);

    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color * spec);
    ourShader.setFloat("material.shininess", shininess);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void drawCube1(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y, glm::vec4 color, float spec, float shininess)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateYMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);

    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color * spec);
    ourShader.setFloat("material.shininess", shininess);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void Floor (Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model;

    // Compacted compound: X from -3 to 36 (width=39), Z from -16 to 16 (depth=32)
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, 1.7f, -18.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(78.0f, 0.1f, 64.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Road(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 1.25f + 0.45f, -16.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.5f, 0.1f, 60.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void garden(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 1.25f + 0.45f, -16.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.5f, 0.1f, 10.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Boundary(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model;

    if (!textureOn)
        glBindTexture(GL_TEXTURE_2D, texture3);
    else
        glBindTexture(GL_TEXTURE_2D, texture0);

    // Compound extents
    const float wallH   = 2.2f;
    const float wallThk = 0.2f;
    const float yBase   = -0.42f;
    const float xMin    = -2.5f;
    const float xMax    = 42.5f;
    const float zBack   = -20.0f;
    const float zFront  =  20.0f;
    const float width   = xMax - xMin;   // 45
    const float depth   = zFront - zBack; // 40

    auto drawWall = [&](float tx, float ty, float tz, float sx, float sy, float sz) {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
        scaleMatrix     = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
        model           = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  color);
        ourShader.setVec4("material.diffuse",  color);
        ourShader.setVec4("material.specular", color * 0.5f);
        ourShader.setFloat("material.shininess", 32.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    // Back wall  (Z = zBack, spans full width)
    drawWall(xMin, yBase, zBack, width, wallH, wallThk);

    // Left wall  (X = xMin, spans full depth)
    drawWall(xMin, yBase, zBack, wallThk, wallH, depth);

    // Right wall (X = xMax, spans full depth)
    drawWall(xMax, yBase, zBack, wallThk, wallH, depth);

    // Front wall (Z = zFront) — two halves with a 5-unit entrance gap centered at X=21
    const float gapHalfW    = 2.5f;
    const float gapCenterX  = 21.0f;
    const float leftFrontW  = (gapCenterX - gapHalfW) - xMin;  // xMin to 18.5
    const float rightFrontX = gapCenterX + gapHalfW;            // 23.5
    const float rightFrontW = xMax - rightFrontX;               // 23.5 to xMax
    drawWall(xMin,       yBase, zFront, leftFrontW,  wallH, wallThk);
    drawWall(rightFrontX, yBase, zFront, rightFrontW, wallH, wallThk);

    // Entry pillars flanking the gap
    glBindTexture(GL_TEXTURE_2D, texture0);
    auto drawPillar = [&](float tx, float ty, float tz, float sx, float sy, float sz) {
        translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
        scaleMatrix     = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
        model           = translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  color * 0.5f);
        ourShader.setVec4("material.diffuse",  color * 0.5f);
        ourShader.setVec4("material.specular", color * 0.25f);
        ourShader.setFloat("material.shininess", 32.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };
    // Left pillar (just left of gap)
    drawPillar(gapCenterX - gapHalfW - 0.6f, yBase, zFront - 0.2f, 0.6f, wallH * 1.2f, 0.6f);
    // Right pillar (just right of gap)
    drawPillar(rightFrontX,                  yBase, zFront - 0.2f, 0.6f, wallH * 1.2f, 0.6f);
}


void Shop(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color, unsigned int texture = texture0)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    // Apply local scaling (1.5x) but leave translations for the caller
    glm::mat4 baseShopOffset = glm::scale(identityMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
    moveMatrix = moveMatrix * baseShopOffset;

    glm::mat4 translateMatrix, scaleMatrix, rotateZMatrix, model;
    glBindVertexArray(cubeVAO);

    glm::vec4 woodColor   = glm::vec4(0.50f, 0.32f, 0.18f, 1.0f);
    glm::vec4 panelColor  = glm::vec4(0.30f, 0.20f, 0.12f, 1.0f);
    glm::vec4 counterColor= glm::vec4(0.42f, 0.27f, 0.10f, 1.0f);
    glm::vec4 roofColor   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col, unsigned int tex, float rotX = 0.0f) {
        glBindTexture(GL_TEXTURE_2D, tex);
        model = glm::translate(identityMatrix, pos);
        if (rotX != 0.0f) {
            model = glm::rotate(model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        model = glm::scale(model, size * 2.0f);
        
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",   col);
        ourShader.setVec4("material.diffuse",   col);
        ourShader.setVec4("material.specular",  col * 0.4f);
        ourShader.setFloat("material.shininess", 32.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    // 1. Four Support Posts (width 0.1, height 1.5, depth 0.1)
    drawBox(glm::vec3(-1.5f, 0.0f, -1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3( 1.5f, 0.0f, -1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3(-1.5f, 0.0f,  1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3( 1.5f, 0.0f,  1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);

    // Floor Platform
    glm::vec4 floorColor = glm::vec4(0.55f, 0.38f, 0.22f, 1.0f);
    drawBox(glm::vec3(-1.6f, -0.05f, -1.1f), glm::vec3(3.2f, 0.05f, 2.3f), floorColor, texture0);

    // Front entrance step
    glm::vec4 stepColor = glm::vec4(0.45f, 0.30f, 0.15f, 1.0f);
    drawBox(glm::vec3(-1.3f, -0.05f, 1.0f), glm::vec3(2.6f, 0.05f, 0.4f), stepColor, texture0);

    // 2. Back Wall Panel
    drawBox(glm::vec3(-1.45f, 0.5f, -0.95f), glm::vec3(2.9f, 1.0f, 0.05f), panelColor, texture0);

    // Side Walls
    drawBox(glm::vec3(-1.48f, 0.0f, -0.95f), glm::vec3(0.05f, 1.5f, 2.0f), panelColor, texture0);
    drawBox(glm::vec3( 1.43f, 0.0f, -0.95f), glm::vec3(0.05f, 1.5f, 2.0f), panelColor, texture0);

    // Horizontal decorative bars on upper open section of side walls
    glm::vec4 barColor = woodColor * 0.9f;
    // Left side
    drawBox(glm::vec3(-1.50f, 0.85f, -0.95f), glm::vec3(0.06f, 0.04f, 1.83f), barColor, texture0);
    drawBox(glm::vec3(-1.50f, 1.10f, -0.95f), glm::vec3(0.06f, 0.04f, 1.83f), barColor, texture0);
    // Right side
    drawBox(glm::vec3( 1.44f, 0.85f, -0.95f), glm::vec3(0.06f, 0.04f, 1.83f), barColor, texture0);
    drawBox(glm::vec3( 1.44f, 1.10f, -0.95f), glm::vec3(0.06f, 0.04f, 1.83f), barColor, texture0);

    // 3. Three Shelves on back wall
    glm::vec4 shelfColor = woodColor * 0.85f;
    float shelfY[3] = { 0.65f, 0.85f, 1.05f };
    for (int i = 0; i < 3; i++) {
        drawBox(glm::vec3(-1.45f, shelfY[i], -0.90f), glm::vec3(2.9f, 0.03f, 0.12f), shelfColor, texture0);
    }

    // Product Boxes on Shelves
    glm::vec4 productColors[3] = {
        glm::vec4(0.8f, 0.2f, 0.2f, 1.0f),  // red
        glm::vec4(0.2f, 0.6f, 0.2f, 1.0f),  // green
        glm::vec4(0.9f, 0.8f, 0.1f, 1.0f)   // yellow
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            drawBox(
                glm::vec3(-0.9f + j * 0.65f, shelfY[i] + 0.05f, -0.91f),
                glm::vec3(0.12f, 0.10f, 0.08f),
                productColors[j % 3], texture0
            );
        }
    }

    // 4. Counter Surface - lowered
    drawBox(glm::vec3(-1.45f, 0.65f, -0.5f), glm::vec3(2.9f, 0.06f, 1.5f), counterColor, texture0);

    // Counter Base
    drawBox(glm::vec3(-1.45f, 0.0f, 0.94f), glm::vec3(2.9f, 0.65f, 0.06f), panelColor, texture0);

    // Glass Case
    glm::vec4 glassColor = glm::vec4(0.7f, 0.85f, 0.95f, 0.6f);
    drawBox(glm::vec3(-1.0f, 0.66f, 0.93f), glm::vec3(2.0f, 0.25f, 0.03f), glassColor, whiteflower);

    // Sack/bag on counter (left side)
    glm::vec4 sackColor = glm::vec4(0.65f, 0.50f, 0.30f, 1.0f);
    drawBox(glm::vec3(-1.2f, 0.68f, 0.10f), glm::vec3(0.18f, 0.20f, 0.15f), sackColor, texture);
    drawBox(glm::vec3(-0.85f, 0.68f, 0.05f), glm::vec3(0.14f, 0.22f, 0.14f), sackColor * 0.85f, texture);

    // Small clay pots on counter (center)
    glm::vec4 potColor = glm::vec4(0.65f, 0.30f, 0.15f, 1.0f);
    drawBox(glm::vec3(-0.15f, 0.68f, 0.10f), glm::vec3(0.10f, 0.14f, 0.10f), potColor, texture0);
    drawBox(glm::vec3( 0.10f, 0.68f, 0.20f), glm::vec3(0.08f, 0.12f, 0.08f), potColor * 1.1f, texture0);
    drawBox(glm::vec3( 0.30f, 0.68f, 0.05f), glm::vec3(0.09f, 0.15f, 0.09f), potColor * 0.9f, texture0);

    // Stacked boxes on right side of counter
    glm::vec4 crateColor = glm::vec4(0.50f, 0.38f, 0.20f, 1.0f);
    drawBox(glm::vec3( 0.80f, 0.68f, 0.10f), glm::vec3(0.20f, 0.12f, 0.18f), crateColor, texture0);
    drawBox(glm::vec3( 0.82f, 0.80f, 0.12f), glm::vec3(0.17f, 0.12f, 0.15f), crateColor * 0.85f, texture0);

    // Hanging produce/items from roof beam (inside shop)
    glm::vec4 produceColor = glm::vec4(0.85f, 0.60f, 0.10f, 1.0f);
    for (int i = 0; i < 4; i++) {
        float hx = -0.9f + i * 0.55f;
        // Thin rope
        drawBox(glm::vec3(hx, 1.50f, -0.30f), glm::vec3(0.02f, 0.20f, 0.02f), woodColor * 0.7f, texture0);
        // Item hanging
        drawBox(glm::vec3(hx - 0.05f, 1.28f, -0.35f), glm::vec3(0.10f, 0.10f, 0.10f), produceColor, texture0);
    }

    // Hanging sign
    glm::vec4 signColor = glm::vec4(0.85f, 0.65f, 0.20f, 1.0f);
    drawBox(glm::vec3(-0.8f, 1.6f, 0.98f), glm::vec3(1.6f, 0.35f, 0.04f), signColor, texture0);
    drawBox(glm::vec3(-0.75f, 1.55f, 0.99f), glm::vec3(0.03f, 0.15f, 0.03f), woodColor, texture0);
    drawBox(glm::vec3( 0.72f, 1.55f, 0.99f), glm::vec3(0.03f, 0.15f, 0.03f), woodColor, texture0);

    // Hanging lanterns
    glm::vec4 lanternColor = glm::vec4(0.9f, 0.7f, 0.1f, 1.0f);
    drawBox(glm::vec3(-0.8f, 1.85f, -0.04f), glm::vec3(0.03f, 0.25f, 0.03f), woodColor, texture0);
    drawBox(glm::vec3(-0.85f, 1.60f, -0.09f), glm::vec3(0.10f, 0.12f, 0.10f), lanternColor, texture0);
    drawBox(glm::vec3( 0.77f, 1.85f, -0.04f), glm::vec3(0.03f, 0.25f, 0.03f), woodColor, texture0);
    drawBox(glm::vec3( 0.72f, 1.60f, -0.09f), glm::vec3(0.10f, 0.12f, 0.10f), lanternColor, texture0);

    // Front overhang trim
    glm::vec4 trimColor = glm::vec4(0.55f, 0.35f, 0.10f, 1.0f);
    drawBox(glm::vec3(-1.7f, 1.46f, 0.95f), glm::vec3(3.4f, 0.06f, 0.08f), trimColor, texture0);

    // 5. Roof Sheets - WIDER
    // Back slope
    drawBox(glm::vec3(-2.0f, 1.5f, -1.5f), glm::vec3(4.0f, 0.07f, 1.6f), roofColor, texture, -22.6f);
    // Front slope
    drawBox(glm::vec3(-2.0f, 2.0f,  0.0f), glm::vec3(4.0f, 0.07f, 1.6f), roofColor, texture, +22.6f);
    
    // Ridge beam
    drawBox(glm::vec3(-2.0f, 1.99f, -0.12f), glm::vec3(4.0f, 0.12f, 0.24f), woodColor, texture0);

    glBindVertexArray(0);
}

void ShopVariant(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 roofTint, unsigned int texture = texture0)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 baseShopOffset = glm::scale(identityMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
    moveMatrix = moveMatrix * baseShopOffset;

    glm::mat4 model;
    glBindVertexArray(cubeVAO);

    glm::vec4 woodColor  = glm::vec4(0.45f, 0.28f, 0.12f, 1.0f);
    glm::vec4 panelColor = glm::vec4(0.28f, 0.18f, 0.10f, 1.0f);

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col, unsigned int tex, float rotX = 0.0f) {
        glBindTexture(GL_TEXTURE_2D, tex);
        model = glm::translate(identityMatrix, pos);
        if (rotX != 0.0f)
            model = glm::rotate(model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, size * 2.0f);
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.3f);
        ourShader.setFloat("material.shininess", 16.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    // Posts
    drawBox(glm::vec3(-1.5f, 0.0f, -1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3( 1.5f, 0.0f, -1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3(-1.5f, 0.0f,  1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);
    drawBox(glm::vec3( 1.5f, 0.0f,  1.0f), glm::vec3(0.12f, 1.5f, 0.12f), woodColor, texture0);

    // Back wall - full height, open top variation (no upper panel)
    drawBox(glm::vec3(-1.45f, 0.0f, -0.95f), glm::vec3(2.9f, 0.75f, 0.05f), panelColor, texture0);

    // Side half walls
    drawBox(glm::vec3(-1.48f, 0.0f, -0.95f), glm::vec3(0.05f, 0.75f, 1.83f), panelColor, texture0);
    drawBox(glm::vec3( 1.43f, 0.0f, -0.95f), glm::vec3(0.05f, 0.75f, 1.83f), panelColor, texture0);

    // Wide display table instead of counter (open stall style)
    glm::vec4 tableColor = glm::vec4(0.48f, 0.30f, 0.12f, 1.0f);
    drawBox(glm::vec3(-1.45f, 0.60f, -0.5f), glm::vec3(2.9f, 0.05f, 1.6f), tableColor, texture0);
    // Table legs
    drawBox(glm::vec3(-1.40f, 0.0f, -0.45f), glm::vec3(0.08f, 0.60f, 0.08f), woodColor, texture0);
    drawBox(glm::vec3( 1.35f, 0.0f, -0.45f), glm::vec3(0.08f, 0.60f, 0.08f), woodColor, texture0);
    drawBox(glm::vec3(-1.40f, 0.0f,  1.00f), glm::vec3(0.08f, 0.60f, 0.08f), woodColor, texture0);
    drawBox(glm::vec3( 1.35f, 0.0f,  1.00f), glm::vec3(0.08f, 0.60f, 0.08f), woodColor, texture0);

    // Produce on table - colorful items
    glm::vec4 colors[6] = {
        glm::vec4(0.9f, 0.2f, 0.1f, 1.0f),  // red (tomato)
        glm::vec4(0.9f, 0.7f, 0.0f, 1.0f),  // yellow (mango)
        glm::vec4(0.2f, 0.65f, 0.2f, 1.0f), // green (veg)
        glm::vec4(0.8f, 0.4f, 0.0f, 1.0f),  // orange
        glm::vec4(0.6f, 0.15f, 0.5f, 1.0f), // purple (brinjal)
        glm::vec4(0.95f, 0.90f, 0.2f, 1.0f) // bright yellow
    };
    for (int col = 0; col < 6; col++) {
        for (int row = 0; row < 2; row++) {
            drawBox(
                glm::vec3(-1.1f + col * 0.40f, 0.63f + row * 0.10f, -0.20f + row * 0.25f),
                glm::vec3(0.13f, 0.09f, 0.13f),
                colors[col], texture0
            );
        }
    }

    // Back support beam (high end)
    drawBox(glm::vec3(-2.0f, 1.80f, -1.0f), glm::vec3(4.0f, 0.08f, 0.08f), woodColor, texture0);
    // Front support beam (low end)  
    drawBox(glm::vec3(-2.0f, 1.40f,  1.0f), glm::vec3(4.0f, 0.08f, 0.08f), woodColor, texture0);

    // Roof panel - translate to back beam height, rotate down toward front
    drawBox(glm::vec3(-2.0f, 1.82f, -1.0f), glm::vec3(4.0f, 0.06f, 2.15f), roofTint, texture, 11.0f);

    glBindVertexArray(0);
}

void Tree(Shader ourShader, glm::mat4 moveMatrix, float trunkHeight = 1.0f,
          glm::vec4 leafColor = glm::vec4(0.13f, 0.45f, 0.13f, 1.0f))
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 model;
    glm::vec4 trunkColor = glm::vec4(0.35f, 0.20f, 0.08f, 1.0f);

    // Draw trunk using cubeVAO
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, texture0);
    model = glm::translate(identityMatrix, glm::vec3(-0.10f, 0.0f, -0.10f));
    model = glm::scale(model, glm::vec3(0.30f, trunkHeight * 2.0f, 0.30f));
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient",   trunkColor);
    ourShader.setVec4("material.diffuse",   trunkColor);
    ourShader.setVec4("material.specular",  glm::vec4(0.05f));
    ourShader.setFloat("material.shininess", 4.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Draw spheres using sphereVAO
    glBindVertexArray(sphereVAO);
    glBindTexture(GL_TEXTURE_2D, texture0);

    auto drawSphere = [&](glm::vec3 pos, float radius, glm::vec4 col) {
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, glm::vec3(radius));
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",   col);
        ourShader.setVec4("material.diffuse",   col);
        ourShader.setVec4("material.specular",  col * 0.08f);
        ourShader.setFloat("material.shininess", 4.0f);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    };

    float base = trunkHeight * 0.75f;

    // Bottom 3 spheres side by side
    drawSphere(glm::vec3( 0.0f, base,        0.45f), 0.50f, leafColor);
    drawSphere(glm::vec3( 0.45f, base,       0.0f),  0.50f, leafColor * 0.95f);
    drawSphere(glm::vec3(-0.45f, base,       0.0f),  0.50f, leafColor * 0.92f);

    // Middle layer - 1 sphere centered, slightly higher, overlapping bottom
    drawSphere(glm::vec3( 0.0f, base + 0.45f, 0.0f), 0.45f, leafColor * 0.88f);

    // Top single sphere
    drawSphere(glm::vec3( 0.0f, base + 0.85f, 0.0f), 0.32f, leafColor * 0.80f);

    glBindVertexArray(0);
}

void BalloonBunch(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 model;

    glm::vec4 balloonColors[6] = {
        glm::vec4(1.0f, 0.15f, 0.15f, 1.0f), // red
        glm::vec4(1.0f, 0.80f, 0.00f, 1.0f), // yellow
        glm::vec4(0.20f, 0.50f, 1.00f, 1.0f), // blue
        glm::vec4(0.15f, 0.80f, 0.30f, 1.0f), // green
        glm::vec4(0.90f, 0.20f, 0.90f, 1.0f), // pink
        glm::vec4(1.00f, 0.50f, 0.10f, 1.0f), // orange
    };

    // Balloon positions - clustered but offset
    glm::vec3 balloonPos[6] = {
        glm::vec3( 0.00f, 2.00f,  0.00f),
        glm::vec3( 0.30f, 2.20f,  0.20f),
        glm::vec3(-0.30f, 2.15f,  0.10f),
        glm::vec3( 0.15f, 2.40f, -0.20f),
        glm::vec3(-0.20f, 2.35f, -0.10f),
        glm::vec3( 0.05f, 2.60f,  0.05f),
    };

    auto drawSphere = [&](glm::vec3 pos, float radius, glm::vec4 col) {
        glBindVertexArray(sphereVAO);
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, glm::vec3(radius));
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",   col);
        ourShader.setVec4("material.diffuse",   col);
        ourShader.setVec4("material.specular",  col * 0.6f); // shiny - balloons are glossy
        ourShader.setFloat("material.shininess", 64.0f);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    };

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, texture0);

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col) {
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, size * 2.0f);
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.1f);
        ourShader.setFloat("material.shininess", 4.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    glm::vec4 stringColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    glm::vec4 stickColor  = glm::vec4(0.55f, 0.38f, 0.20f, 1.0f);

    // Stick held by vendor
    drawBox(glm::vec3(-0.03f, 0.0f, -0.03f), glm::vec3(0.04f, 1.0f, 0.04f), stickColor);

    // Strings from stick tip to each balloon
    for (int i = 0; i < 6; i++) {
        glm::vec3 strPos = glm::vec3(
            balloonPos[i].x * 0.5f,
            0.9f + (balloonPos[i].y - 2.0f) * 0.4f,
            balloonPos[i].z * 0.5f
        );
        drawBox(strPos, glm::vec3(0.01f, 0.25f, 0.01f), stringColor);
    }

    // Balloons
    for (int i = 0; i < 6; i++) {
        drawSphere(balloonPos[i], 0.22f, balloonColors[i]);
        // Small knot at bottom of each balloon
        drawBox(
            glm::vec3(balloonPos[i].x, balloonPos[i].y - 0.20f, balloonPos[i].z),
            glm::vec3(0.03f, 0.04f, 0.03f),
            balloonColors[i] * 0.7f
        );
    }

    glBindVertexArray(0);
}

void FlagBunting(Shader ourShader, glm::mat4 moveMatrix, float length = 6.0f)
{
    static unsigned int flagVAO = 0;
    if (flagVAO == 0) {
        float flagVertices[] = {
            // Front face
            -0.18f, 0.0f,  0.0f,  0.0f,0.0f,1.0f,  0.0f,1.0f, // top left
             0.18f, 0.0f,  0.0f,  0.0f,0.0f,1.0f,  1.0f,1.0f, // top right
             0.0f, -0.4f,  0.0f,  0.0f,0.0f,1.0f,  0.5f,0.0f, // bottom point
            
            // Back face
             0.18f, 0.0f,  0.0f,  0.0f,0.0f,-1.0f, 1.0f,1.0f, // top right
            -0.18f, 0.0f,  0.0f,  0.0f,0.0f,-1.0f, 0.0f,1.0f, // top left
             0.0f, -0.4f,  0.0f,  0.0f,0.0f,-1.0f, 0.5f,0.0f  // bottom
        };
        unsigned int flagVBO;
        glGenVertexArrays(1, &flagVAO);
        glGenBuffers(1, &flagVBO);
        glBindVertexArray(flagVAO);
        glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(flagVertices), flagVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 model;

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col) {
        glBindVertexArray(cubeVAO);
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, size * 2.0f);
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.1f);
        ourShader.setFloat("material.shininess", 8.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    glm::vec4 poleColor = glm::vec4(0.35f, 0.22f, 0.10f, 1.0f);
    glm::vec4 ropeColor = glm::vec4(0.70f, 0.65f, 0.50f, 1.0f);

    glm::vec4 flagColors[5] = {
        glm::vec4(0.95f, 0.15f, 0.15f, 1.0f), // red
        glm::vec4(0.20f, 0.45f, 0.90f, 1.0f), // blue
        glm::vec4(0.95f, 0.85f, 0.05f, 1.0f), // yellow
        glm::vec4(0.15f, 0.75f, 0.30f, 1.0f), // green
        glm::vec4(0.90f, 0.30f, 0.80f, 1.0f), // pink
    };

    glBindTexture(GL_TEXTURE_2D, texture0);

    // Support poles
    drawBox(glm::vec3(0.0f,    0.0f, -0.02f), glm::vec3(0.04f, 2.0f, 0.04f), poleColor);
    drawBox(glm::vec3(length,  0.0f, -0.02f), glm::vec3(0.04f, 2.0f, 0.04f), poleColor);

    // Horizontal rope line hanging exactly at height 1.95
    float ropeY = 1.95f;
    drawBox(glm::vec3(0.0f, ropeY, 0.0f), glm::vec3(length, 0.015f, 0.015f), ropeColor);

    // Draw custom downward-pointing triangle flags
    glBindVertexArray(flagVAO);
    int numFlags = (int)(length / 0.5f);
    for (int i = 0; i < numFlags; i++) {
        float fx = 0.25f + i * 0.5f;
        glm::vec4 col = flagColors[i % 5];

        // Origin of triangle is the flat top line.
        model = glm::translate(identityMatrix, glm::vec3(fx, ropeY + 0.01f, 0.0f));
        // Add a slight wind rotation 
        model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.1f);
        ourShader.setFloat("material.shininess", 8.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
}

void VendorCart(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 model;
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glm::vec4 bodyColor   = glm::vec4(0.60f, 0.35f, 0.12f, 1.0f);
    glm::vec4 rimColor    = glm::vec4(0.25f, 0.15f, 0.05f, 1.0f);
    glm::vec4 wheelColor  = glm::vec4(0.18f, 0.12f, 0.06f, 1.0f);
    glm::vec4 axleColor   = glm::vec4(0.30f, 0.20f, 0.10f, 1.0f);
    glm::vec4 handleColor = glm::vec4(0.40f, 0.25f, 0.10f, 1.0f);
    glm::vec4 canopyColor = glm::vec4(0.85f, 0.25f, 0.15f, 1.0f); // red canopy

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col, float rotX = 0.0f, float rotY = 0.0f) {
        model = glm::translate(identityMatrix, pos);
        if (rotX != 0.0f)
            model = glm::rotate(model, glm::radians(rotX), glm::vec3(1,0,0));
        if (rotY != 0.0f)
            model = glm::rotate(model, glm::radians(rotY), glm::vec3(0,1,0));
        model = glm::scale(model, size * 2.0f);
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.3f);
        ourShader.setFloat("material.shininess", 16.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    auto drawSphere = [&](glm::vec3 pos, float radius, glm::vec4 col) {
        glBindVertexArray(sphereVAO);
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, glm::vec3(radius));
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",   col);
        ourShader.setVec4("material.diffuse",   col);
        ourShader.setVec4("material.specular",  col * 0.4f);
        ourShader.setFloat("material.shininess", 32.0f);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, texture0);
    };

    // Cart body - main box
    drawBox(glm::vec3(-0.7f, 0.45f, -0.45f), glm::vec3(1.4f, 0.35f, 0.90f), bodyColor);

    // Body rim (darker edge around top)
    drawBox(glm::vec3(-0.72f, 0.78f, -0.47f), glm::vec3(1.44f, 0.04f, 0.94f), rimColor);

    // Cart floor inside
    drawBox(glm::vec3(-0.68f, 0.46f, -0.43f), glm::vec3(1.36f, 0.02f, 0.86f), bodyColor * 0.8f);

    // Items on cart - small colorful goods
    glm::vec4 itemColors[4] = {
        glm::vec4(0.9f, 0.8f, 0.1f, 1.0f),
        glm::vec4(0.8f, 0.3f, 0.1f, 1.0f),
        glm::vec4(0.3f, 0.7f, 0.3f, 1.0f),
        glm::vec4(0.9f, 0.5f, 0.1f, 1.0f),
    };
    for (int i = 0; i < 4; i++) {
        drawSphere(
            glm::vec3(-0.45f + i * 0.28f, 0.90f, 0.0f),
            0.12f, itemColors[i]
        );
    }

    // Axle
    drawBox(glm::vec3(-0.75f, 0.22f, -0.02f), glm::vec3(1.5f, 0.04f, 0.04f), axleColor);

    // Two wheels (sphere approximation - flat cylinder = squashed sphere)
    drawSphere(glm::vec3(-0.72f, 0.22f, 0.0f), 0.22f, wheelColor);
    drawSphere(glm::vec3( 0.72f, 0.22f, 0.0f), 0.22f, wheelColor);

    // Wheel spokes (4 per wheel - flat cross boxes)
    for (int w = 0; w < 2; w++) {
        float wx = (w == 0) ? -0.72f : 0.72f;
        // Horizontal spoke
        drawBox(glm::vec3(wx - 0.01f, 0.22f, -0.18f), glm::vec3(0.02f, 0.02f, 0.18f), rimColor);
        // Vertical spoke
        drawBox(glm::vec3(wx - 0.01f, 0.05f, -0.01f), glm::vec3(0.02f, 0.18f, 0.02f), rimColor);
    }

    // Handle poles (two parallel bars going backward)
    drawBox(glm::vec3(-0.55f, 0.55f, 0.45f), glm::vec3(0.04f, 0.04f, 0.50f), handleColor);
    drawBox(glm::vec3( 0.55f, 0.55f, 0.45f), glm::vec3(0.04f, 0.04f, 0.50f), handleColor);
    // Handle grip connecting the two bars
    drawBox(glm::vec3(-0.55f, 0.55f, 0.94f), glm::vec3(1.10f, 0.04f, 0.04f), handleColor);

    // Canopy poles
    drawBox(glm::vec3(-0.65f, 0.80f, -0.40f), glm::vec3(0.04f, 0.70f, 0.04f), handleColor);
    drawBox(glm::vec3( 0.65f, 0.80f, -0.40f), glm::vec3(0.04f, 0.70f, 0.04f), handleColor);
    drawBox(glm::vec3(-0.65f, 0.80f,  0.40f), glm::vec3(0.04f, 0.70f, 0.04f), handleColor);
    drawBox(glm::vec3( 0.65f, 0.80f,  0.40f), glm::vec3(0.04f, 0.70f, 0.04f), handleColor);

    // Canopy roof - flat single slope
    drawBox(glm::vec3(-0.75f, 1.50f, -0.50f), glm::vec3(1.50f, 0.04f, 1.00f), canopyColor);
    // Canopy front overhang
    drawBox(glm::vec3(-0.75f, 1.45f,  0.45f), glm::vec3(1.50f, 0.04f, 0.12f), canopyColor * 0.85f);

    glBindVertexArray(0);
}

void Well(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 model;
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glm::vec4 stoneColor = glm::vec4(0.60f, 0.58f, 0.52f, 1.0f);
    glm::vec4 darkStone  = glm::vec4(0.40f, 0.38f, 0.34f, 1.0f);
    glm::vec4 woodColor  = glm::vec4(0.45f, 0.28f, 0.10f, 1.0f);
    glm::vec4 ropeColor  = glm::vec4(0.65f, 0.58f, 0.40f, 1.0f);

    auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col) {
        model = glm::translate(identityMatrix, pos);
        model = glm::scale(model, size * 2.0f);
        ourShader.setMat4("model", moveMatrix * model);
        ourShader.setVec4("material.ambient",  col);
        ourShader.setVec4("material.diffuse",  col);
        ourShader.setVec4("material.specular", col * 0.2f);
        ourShader.setFloat("material.shininess", 16.0f);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    };

    // Well base - 4 stone walls forming a ring
    drawBox(glm::vec3(-0.50f, 0.0f, -0.50f), glm::vec3(1.00f, 0.45f, 0.08f), stoneColor); // front
    drawBox(glm::vec3(-0.50f, 0.0f,  0.42f), glm::vec3(1.00f, 0.45f, 0.08f), stoneColor); // back
    drawBox(glm::vec3(-0.50f, 0.0f, -0.42f), glm::vec3(0.08f, 0.45f, 0.84f), darkStone);  // left
    drawBox(glm::vec3( 0.42f, 0.0f, -0.42f), glm::vec3(0.08f, 0.45f, 0.84f), darkStone);  // right

    // Stone cap on top of walls
    drawBox(glm::vec3(-0.55f, 0.43f, -0.55f), glm::vec3(1.10f, 0.05f, 0.12f), darkStone);
    drawBox(glm::vec3(-0.55f, 0.43f,  0.43f), glm::vec3(1.10f, 0.05f, 0.12f), darkStone);
    drawBox(glm::vec3(-0.55f, 0.43f, -0.43f), glm::vec3(0.12f, 0.05f, 0.86f), darkStone);
    drawBox(glm::vec3( 0.43f, 0.43f, -0.43f), glm::vec3(0.12f, 0.05f, 0.86f), darkStone);

    // Two support posts
    drawBox(glm::vec3(-0.52f, 0.45f, -0.06f), glm::vec3(0.07f, 0.60f, 0.07f), woodColor);
    drawBox(glm::vec3( 0.45f, 0.45f, -0.06f), glm::vec3(0.07f, 0.60f, 0.07f), woodColor);

    // Crossbeam (roller beam)
    drawBox(glm::vec3(-0.52f, 1.02f, -0.05f), glm::vec3(0.97f, 0.05f, 0.05f), woodColor);

    // Rope hanging down
    drawBox(glm::vec3(-0.04f, 0.48f, -0.04f), glm::vec3(0.03f, 0.50f, 0.03f), ropeColor);

    // Bucket at bottom of rope
    drawBox(glm::vec3(-0.08f, 0.10f, -0.08f), glm::vec3(0.16f, 0.14f, 0.16f),
            glm::vec4(0.22f, 0.16f, 0.08f, 1.0f));

    glBindVertexArray(0);
}

void Gate(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glBindTexture(GL_TEXTURE_2D, texture0);
    // Rotation
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    //static parts
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.3f, 0.6f, 15.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.05f, 0.05f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.3f, -0.2f, 15.1f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.3f, 0.6f, 15.1f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.3f, -0.2f, 15.1f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    

    //dynamic parts
    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec3 pivot1, pivot2;

    pivot1 = glm::vec3(7.35, 0.0, 15.1125);
    pivot2 = glm::vec3(9.30, 0.0, 15.1125);

    gateAngle += isGateOpening * gateSpeed;
    if (isGateOpening == -1.0f)
        gateAngle = max(0.0f, gateAngle);
    if (isGateOpening == 1.0f)
        gateAngle = min(90.0f, gateAngle);

    //***left side***//
    translateToPivot = glm::translate(identityMatrix, -pivot1);
    translateFromPivot = glm::translate(identityMatrix, pivot1);
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(gateAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    //square
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.34f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.0f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.26f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.5f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.34f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.9f, 0.4f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.34f, 0.65f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.9f, 0.4f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //bars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.52f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.7f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.88f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.06f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



    //***right side//
    translateToPivot = glm::translate(identityMatrix, -pivot2);
    translateFromPivot = glm::translate(identityMatrix, pivot2);
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(-gateAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    //square
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.25f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.0f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.33f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.5f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.33f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.9f, 0.4f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.33f, 0.65f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.9f, 0.4f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix* model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //bars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.07f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix* model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.89f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix* model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.71f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix* model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.53f, -0.3f, 15.09f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.05f, 2.1f, 0.1f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix* model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


}

// LampPost has been moved to include/lamppost.h as drawLampPost().
// It is now a local-space object so callers pass a plain world transform.

void Stage(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y)
{
    glBindTexture(GL_TEXTURE_2D, texture0);
    // Rotation
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec3 pivot;

    pivot = glm::vec3(0.25, 0.25, 0.25);

    //Base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.43f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 1.2f, 3.5f));

    translateToPivotOrg = scaleMatrix * glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = scaleMatrix * glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;

    //model = translateMatrix * rotateYMatrix * scaleMatrix;
    //model = translateMatrix * scaleMatrix;
    glm::vec4 clr = glm::vec4(100.0/256, 100.0/256, 100/256, 1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", clr);
    ourShader.setVec4("material.diffuse", clr);
    ourShader.setVec4("material.specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    pivot = translateFromPivotOrg;
    // Back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.43f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(5.0f, 3.5f, 0.2f));

    translateToPivotOrg = glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    clr = glm::vec4(17.0 / 256, 17.0 / 256, 20 / 256, 1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", clr);
    ourShader.setVec4("material.diffuse", clr);
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 256.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Stairs
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.6f, -0.43f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.4f, 1.5f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    clr = glm::vec4(43.0 / 256, 22.0 / 256, 4 / 256, 1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", clr);
    ourShader.setVec4("material.diffuse", clr);
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.8f, -0.43f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.8f, 1.5f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(12.5f, -0.43f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.8f, 1.5f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(12.7f, -0.43f, 0.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.4f, 0.4f, 1.5f));
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void FoodPlace(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model;

    //Floor
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -0.44f, 2.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 5.0f));
    model = translateMatrix * scaleMatrix;
    glm::vec4 color = glm::vec4(0.8,0.0,0.7, 1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.44f, 2.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.1f, 5.0f));
    model = translateMatrix * scaleMatrix;
    glm::vec4 color1 = glm::vec4(1.0, 0.0, 0.7, 1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color1);
    ourShader.setVec4("material.diffuse", color1);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -0.44f, 6.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 1.6f, 3.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color1);
    ourShader.setVec4("material.diffuse", color1);
    ourShader.setVec4("material.specular", glm::vec4(0.1f, 0.1f, 0.1f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, 0.0f, 4.0f));
    Stage(ourShader, moveMatrix * translateMatrix, 90);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -10.0f));
    TableChairSet(ourShader, moveMatrix * translateMatrix, rotateAngleTest_Y);

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -8.0f));
    TableChairSet(ourShader, moveMatrix * translateMatrix, rotateAngleTest_Y);


}

void TableChairSet(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y)
{
}