// Ferris Wheel component — #included by main.cpp.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix);
void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z, glm::mat4 parentMove);

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
