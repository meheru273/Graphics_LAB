// Carousel component — #included by main.cpp.
// All global variables and #includes are inherited from main.cpp.
#pragma once

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
