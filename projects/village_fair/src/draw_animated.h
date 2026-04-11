// This file is #included by main.cpp.
// It is part of the Village-fair project split for readability.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix)
{

    //front
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    if (isFerrisWheelOn)
    {
        ferrisWheelSpeed += ferrisIncrement;
        ferrisWheelSpeed = min(2.5f, ferrisWheelSpeed);
    }
    else
    {
        ferrisWheelSpeed -= ferrisIncrement;
        ferrisWheelSpeed = max(0.02f, ferrisWheelSpeed);
    }

    if (ferrisWheelSpeed != 0.02f)
        ferrisWheelAngle += ferrisWheelSpeed;

    ferrisWheelAngle = fmod(ferrisWheelAngle, 360);
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 12.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;

    glm::vec4 color = glm::vec4(100.0 / 256, 100.0 / 256, 200.0 / 256, 1.0f);
    //glm::vec4 color = glm::vec4(1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //bars
    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(0.0f), 1.0f);

    pivot = translateMatrix * pivot;

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 11.975f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 0.1f));
    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }



    //back
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 11.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //bars

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 10.975f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 0.1f));
    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    //connecting bars
    //middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.975f, 1.975f, 10.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 4.0f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //top ones
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.5f, 10.995f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 2.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    for (float i = 0.0f; i >= -360.0f; i -= 90.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //pillars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, 12.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, 12.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(42.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, 10.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, 10.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(42.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    color = glm::vec4(1.0f);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    //sitting places — pass moveMatrix as parentMove (4th arg) so seats follow the wheel
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, moveMatrix);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 1.5f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, moveMatrix);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.0f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, moveMatrix);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 1.5f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle, moveMatrix);


}

void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z, glm::mat4 parentMove)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrixLocal, rotateZMatrixMain, rotateTemp;
    glm::mat4 translateToPivotMain, translateFromPivotMain;
    glm::vec4 translateToPivotOrgMain, translateFromPivotOrgMain;
    glm::mat4 translateToPivotLocal, translateFromPivotLocal;
    glm::vec4 translateToPivotOrgLocal, translateFromPivotOrgLocal;
    glm::vec4 pivot1, pivot2;
    glm::vec4 mainPivotPoint = glm::vec4(3.0f, 2.0f, 0.0f, 1.0);
    glm::vec4 localPivotPoint = glm::vec4(2.975f, 0.475f, -1.005f, 1.0);

    pivot1 = mainPivotPoint;
    translateToPivotMain = glm::translate(identityMatrix, glm::vec3(-pivot1));
    translateFromPivotMain = glm::translate(identityMatrix, glm::vec3(pivot1));
    rotateZMatrixMain = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    pivot2 = translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * moveMatrix * localPivotPoint;
    translateToPivotLocal = glm::translate(identityMatrix, glm::vec3(-pivot2));
    translateFromPivotLocal = glm::translate(identityMatrix, glm::vec3(pivot2));
    rotateZMatrixLocal = glm::rotate(identityMatrix, glm::radians(-rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    glBindTexture(GL_TEXTURE_2D, texture3);
    //base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, texture10);
    //left
    //seat
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.5f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //seat right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.5f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.45f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //top bars
    glBindTexture(GL_TEXTURE_2D, texture0);
    //left
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.899f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.15f, 0.1f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.1751f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.15f, 0.1f, 0.05f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //right
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.899f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.15f, 0.1f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.1751f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.15f, 0.1f, 0.05f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", parentMove * moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


}
void drawKite(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 barColor, float scale, glm::vec3 position) {
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::translate(identityMatrix, position);
    glm::mat4 scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale, scale, scale));
    glm::mat4 model = translateMatrix * scaleMatrix;

    // Set kite properties
    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient", kiteColor);
    shader.setVec4("material.diffuse", kiteColor);
    shader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);

    // Define kite geometry
    float kiteVertices[] = {
        // Positions         // Normals         // Texture Coords
        0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 1.0f,  0.5f, 1.0f, // Top
        -0.5f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.5f, // Left
        0.5f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,  1.0f, 0.5f, // Right
        0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.5f, 0.0f  // Bottom
    };
    unsigned int kiteIndices[] = {
        0, 1, 2, // Upper triangle
        1, 3, 2  // Lower triangle
    };

    // Create VAO, VBO, and EBO for the kite
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kiteVertices), kiteVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kiteIndices), kiteIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw the bars inside the kite
    shader.setVec4("material.ambient", barColor);
    shader.setVec4("material.diffuse", barColor);

    float barVertices[] = {
        // Positions
        0.0f, 1.0f, 0.0f,  // Top
        0.0f, -1.0f, 0.0f, // Bottom
        -0.5f, 0.0f, 0.0f, // Left
        0.5f, 0.0f, 0.0f   // Right
    };
    unsigned int barIndices[] = {
        0, 1, // Vertical bar
        2, 3  // Horizontal bar
    };

    unsigned int barVAO, barVBO, barEBO;
    glGenVertexArrays(1, &barVAO);
    glGenBuffers(1, &barVBO);
    glGenBuffers(1, &barEBO);

    glBindVertexArray(barVAO);

    glBindBuffer(GL_ARRAY_BUFFER, barVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(barVertices), barVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, barEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(barIndices), barIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(barVAO);
    glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &barVAO);
    glDeleteBuffers(1, &barVBO);
    glDeleteBuffers(1, &barEBO);
}
void drawKiteWithStick(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 stickColor, float scale, glm::vec3 position) {
    // Draw the kite
    drawKite(shader, moveMatrix, kiteColor, stickColor, scale, position);

    // Draw the stick
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix = glm::translate(identityMatrix, position + glm::vec3(0.0f, -scale * 1.5f, 0.0f));
    glm::mat4 scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.02f * scale, 3.0f * scale, 0.02f * scale));
    glm::mat4 model = translateMatrix * scaleMatrix;

    shader.setMat4("model", moveMatrix * model);
    shader.setVec4("material.ambient", stickColor);
    shader.setVec4("material.diffuse", stickColor);
    shader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);

    glBindVertexArray(cubeVAO); // Use a cube VAO for the stick
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


void drawJar(Shader& shader, glm::mat4 moveMatrix, glm::vec4 color, float radius, float height, int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices for the cylinder
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Top and bottom circle vertices
        vertices.insert(vertices.end(), { x, height / 2, z, 0.0f, 1.0f, 0.0f }); // Top
        vertices.insert(vertices.end(), { x, -height / 2, z, 0.0f, -1.0f, 0.0f }); // Bottom
    }

    // Indices for the cylinder walls
    for (int i = 0; i < segments; ++i) {
        int top1 = i * 2;
        int top2 = (i * 2 + 2) % (segments * 2);
        int bottom1 = top1 + 1;
        int bottom2 = top2 + 1;

        // Side walls
        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(top2);

        indices.push_back(top2);
        indices.push_back(bottom1);
        indices.push_back(bottom2);
    }


    // Create buffers
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Draw the jar
    shader.setMat4("model", moveMatrix);
    shader.setVec4("material.ambient", color);
    shader.setVec4("material.diffuse", color);
    shader.setVec4("material.specular", glm::vec4(0.5f));
    shader.setFloat("material.shininess", 32.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
// Add this outside the main function, near other rendering helper functions
void drawBezierRiver(Shader& shader, glm::mat4 moveMatrix, unsigned int VAO, std::vector<int>& indices, glm::vec4 color) {
    shader.setMat4("model", moveMatrix);
    shader.setVec4("material.ambient", color);
    shader.setVec4("material.diffuse", color);
    shader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    shader.setFloat("material.shininess", 30.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void PirateShip(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z)
{

    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(14.0f, 2.0f, 4.2f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    glm::vec4 color = glm::vec4(1.0f, 10.0 / 256, 20.0 / 256, 1.0f);

    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastTime > 0.1)
    {
        if (isBoatOn)
            boatAngle += boatSpeed;
        if (abs(boatAngle) >= boatMaxAngle)
        {
            boatSpeed *= -1;
            lastTime = static_cast<float>(glfwGetTime());
        }
    }
    if (!isBoatOn)
    {
        if (boatAngle < 0)
        {
            boatAngle += abs(boatSpeed);
        }
        if (boatAngle > 0)
        {
            boatAngle -= abs(boatSpeed);
        }
    }


    rotateAngleTest_Z = boatAngle;
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    //dynamic
    //boat
    glBindTexture(GL_TEXTURE_2D, texture3);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, -1.18f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 0.7f, 0.3f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //base
    color = glm::vec4(150.0 / 256, 12.0 / 256, 35.0 / 256, 1.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, -0.45f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.55f, 0.2f, 0.248f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, texture0);
    //bars
    color = glm::vec4(1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);

    float moveX, moveY, moveZ;
    moveX = 11.0f, moveY = -0.4f, moveZ = 7.7f;
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.975f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.575f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.375f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    
}

// Function to draw a tree using B�zier curves
void drawRealisticTree(Shader& shader, glm::mat4 modelMatrix, unsigned int trunkVAO, unsigned int canopyVAO, std::vector<int>& trunkIndices, std::vector<int>& canopyIndices) {
    shader.use();

    // Draw trunk
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, -1.0f, 0.25f)); // Position canopy above trunk
    shader.setMat4("model", modelMatrix);
    shader.setVec4("material.ambient", glm::vec4(0.6f, 0.3f, 0.1f, 1.0f)); // Brown color
    shader.setVec4("material.diffuse", glm::vec4(0.6f, 0.3f, 0.1f, 1.0f));
    shader.setVec4("material.specular", glm::vec4(0.1f, 0.05f, 0.05f, 1.0f));
    shader.setFloat("material.shininess", 16.0f);

    glBindVertexArray(trunkVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(trunkIndices.size()), GL_UNSIGNED_INT, 0);

    // Draw canopy
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, -2.0f,0.0f)); // Position canopy above trunk
    shader.setMat4("model", modelMatrix);
    shader.setVec4("material.ambient", glm::vec4(0.6f, 0.4f, 0.8f, 1.0f)); // Purple color for leaves
    shader.setVec4("material.diffuse", glm::vec4(0.6f, 0.4f, 0.8f, 1.0f));
    shader.setVec4("material.specular", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    shader.setFloat("material.shininess", 8.0f);

    glBindVertexArray(canopyVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(canopyIndices.size()), GL_UNSIGNED_INT, 0);
}

void Carousal(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp, rotateYMatrix, rotateTempZ, rotateTempX;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(14.0f, 1.2f, -4.25f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    if (isCarousalOn)
    {
        carousalSpeed += carousalIncrement;
        carousalSpeed = min(3.5f, carousalSpeed);
    }
    else
    {
        carousalSpeed -= carousalIncrement;
        carousalSpeed = max(0.02f, carousalSpeed);
    }

    if (carousalSpeed != 0.02f)
        carousalAngle += carousalSpeed;

    carousalAngle = fmod(carousalAngle, 360);
    rotateAngleTest_Z = -carousalAngle;

    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec4 color = glm::vec4(200.0 / 256, 20.0 / 256, 68.0 / 256, 1.0f);
    //Circle
    glBindTexture(GL_TEXTURE_2D, texture0);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, 1.2f, -4.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateYMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(carousalVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesCarousal.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    color = glm::vec4(112.0 / 256, 161.0 / 256, 151.0 / 256, 1.0f);
    //cylinder
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, -0.46f, -4.25f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.15f, 0.2f));
    model = translateMatrix * rotateYMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(cylinderVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesCylinder.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    //Seats
    color = glm::vec4(0.7f);
    ourShader.setVec4("material.ambient", color * 0.7f);
    ourShader.setVec4("material.diffuse", color * 0.7f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(cubeVAO);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(12.2f, 0.60f, -4.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.4f, 0.1f));
    rotateTempZ = glm::rotate(identityMatrix, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rotateTempX = glm::rotate(identityMatrix, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    for (float i = 0.0f; i <= 360.f; i += 60.0f)
    {
        //handles
        glBindTexture(GL_TEXTURE_2D, texture0);
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 1.0f, 0.0f));
        translateMatrix = glm::translate(identityMatrix, glm::vec3(12.0f, 2.25f, -4.3f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 2.4f, 0.1f));
        model = translateFromPivot * rotateYMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * rotateTempZ * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesCylinder.size(), GL_UNSIGNED_INT, (void*)0);

        //boxes
        glBindTexture(GL_TEXTURE_2D, texture10);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.875f, 2.25f, -4.55f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
        model = translateFromPivot * rotateYMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * rotateTempZ * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesCylinder.size(), GL_UNSIGNED_INT, (void*)0);

        //head
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.6f, 4.55f, -3.70f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 0.5f, 0.7f));
        model = translateFromPivot * rotateYMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * rotateTempX * rotateTempZ * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(headVAO);
        glDrawElements(GL_TRIANGLES, (unsigned int)indicesHead.size(), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);

    }

}

void seat(Shader ourShader, glm::mat4 moveMatrix) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    //seat
    //base
    glm::vec4 color = glm::vec4(1.0f);
    glBindTexture(GL_TEXTURE_2D, texture7);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 0.35f , -11.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.5f, 0.2f, 1.5f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec4(0.5f);
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 0.35f , -11.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.5f, 1.5f, 0.2f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec4(0.5f);
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //left handle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 0.35f , -11.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 1.5f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec4(0.5f);
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //right handle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.65f, 0.35f , -11.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 1.5f));
    model = translateMatrix * scaleMatrix;
    color = glm::vec4(0.5f);
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    


}
void glass(Shader& ourShader, unsigned int& VAO, float tx, float ty, float tz, float sx, float sy, float sz)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translateMatrix, scaleMatrix, model;
    
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(.025f, .3f, .025f));
    model = translateMatrix * scaleMatrix;
    glm::vec4 color = glm::vec4(0.039f, 0.79f, 0.96f, 0.0f);
    ourShader.setMat4("model", model);
    ourShader.setVec4("material.ambient", color); // Glassy material
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 200.0f);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);
}