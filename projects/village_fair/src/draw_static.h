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
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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


void Table(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y)
{

    float tableY = 0.05f;
    // Rotation
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec3 pivot;

    pivot = glm::vec3(0.25, 0.25 + tableY, 0.25);
    
    //Top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f + tableY, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 1.3f));

    translateToPivotOrg = scaleMatrix * glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = scaleMatrix * glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    
    //model = translateMatrix * rotateYMatrix * scaleMatrix;
    //model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix*model);
    ourShader.setVec4("material.ambient", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.5f, 1.0f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    ourShader.setFloat("material.shininess", 128.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    pivot = translateFromPivotOrg;
    // Leg 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.02f, -0.46f + tableY, 0.01f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 0.1f));

    translateToPivotOrg = glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));
    
    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // Leg 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.02f, -0.46f + tableY, 0.559f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 0.1f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // Leg 3
    //glm::mat4 translateMatrix1, rotateXMatrix1, rotateYMatrix1, rotateZMatrix1, scaleMatrix1, model1;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.92f, -0.46f + tableY, 0.01f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 0.1f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // Leg 4
    //glm::mat4 translateMatrix2, rotateXMatrix2, rotateYMatrix2, rotateZMatrix2, scaleMatrix2, model2;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.92f, -0.46f + tableY, 0.559f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 0.1f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Chair(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;
    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec3 pivot;

    float fixLeg = 0.7f;
    float fixY = 0.1f;
    float fixYWhole = -0.08f;

    pivot = glm::vec3(0.25 + 0.11, 0.25 - fixY + fixYWhole, 0.25);

    // Top
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.11f, 0.0f - fixY + fixYWhole, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.75f, 0.07f, 0.7f));

    translateToPivotOrg = scaleMatrix * glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = scaleMatrix * glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.3f, 0.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.3f, 0.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    pivot = translateFromPivotOrg;
    // Leg 1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, -0.32f + fixYWhole, 0.01f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.7f * fixLeg, 0.07f));

    translateToPivotOrg = glm::vec4(-pivot, 0.0f);
    translateFromPivotOrg = glm::vec4(pivot, 0.0f);
    translateToPivot = glm::translate(identityMatrix, glm::vec3(translateToPivotOrg));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(translateFromPivotOrg));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // Leg 2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.13f, -0.32f + fixYWhole, 0.28f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.7f * fixLeg, 0.07f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    /*ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);*/
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //// Leg 3
    //glm::mat4 translateMatrix1, rotateXMatrix1, rotateYMatrix1, rotateZMatrix1, scaleMatrix1, model1;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.43f, -0.32f + fixYWhole, 0.01f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.7f * fixLeg, 0.07f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    /*ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);*/
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    // Leg 4
    //glm::mat4 translateMatrix2, rotateXMatrix2, rotateYMatrix2, rotateZMatrix2, scaleMatrix2, model2;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.43f, -0.32f + fixYWhole, 0.28f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.7f * fixLeg, 0.07f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    /*ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);*/
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Left up 
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.17f, 0.01f - fixY + fixYWhole, 0.29f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.25f, 0.07f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Right up
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.39f, 0.01f - fixY + fixYWhole, 0.29f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.07f, 0.25f, 0.07f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Back support
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.15f, 0.09f - fixY + fixYWhole, 0.28f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.5f, 0.1f));

    model = translateFromPivot * rotateYMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.9f, 0.3f, 1.0f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.9f, 0.3f, 1.0f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Floor (Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 1.25f + 0.45f, -16.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(40.3f, 0.1f, 60.0f));
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
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model;

    //Left Side
    if (!textureOn)
    {
        glBindTexture(GL_TEXTURE_2D, texture3);
    }
    else
    {

        glBindTexture(GL_TEXTURE_2D, texture0);
    }
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.42f, 15.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(18.2f, 2.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color * 0.5f);
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //Left Wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.42f, 15.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 2.2f, -57.8f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Right Side
    translateMatrix = glm::translate(identityMatrix, glm::vec3(18.15f, -0.42f, 15.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-17.2f, 2.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //Right Wall
    translateMatrix = glm::translate(identityMatrix, glm::vec3(18.15f, -0.42f, 15.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-0.2f, 2.2f, -57.8f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, -0.42f, -14.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(19.2f, 2.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(18.15f, -0.42f, -14.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-21.2f, 2.2f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    //Left Side Pillar
    glBindTexture(GL_TEXTURE_2D, texture0);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f, -0.42f, 14.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 2.6f, 0.6f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color * 0.5f * 0.5f);
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //Right Side Pillar
    translateMatrix = glm::translate(identityMatrix, glm::vec3(9.35f, -0.42f, 14.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 2.6f, 0.6f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}


void Shop(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color, unsigned int texture = texture0)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix;


    //roof
    glBindTexture(GL_TEXTURE_2D, whiteflower);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.7f, 0.95f, 12.86f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(-4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.8f, 0.29f, 6.0f));
    model = translateMatrix * rotateZMatrix *scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color );
    ourShader.setVec4("material.diffuse", color );
    ourShader.setVec4("material.specular", color );
    ourShader.setFloat("material.shininess", 256.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //back side
    glBindTexture(GL_TEXTURE_2D, texture);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.4f, -0.42f, 13.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 2.75f, 5.5f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //right side
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color * 0.5f);
    ourShader.setFloat("material.shininess", 32.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.99f, -0.419f, 12.99f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 2.75f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //left side
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color * 0.5f);
    ourShader.setFloat("material.shininess", 32.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(6.99f, -0.419f, 15.651f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(3.0f, 2.75f, 0.2f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //front side
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f, -0.42f, 13.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 1.2f, 4.0f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //floor
    translateMatrix = glm::translate(identityMatrix, glm::vec3(7.01f, -0.42f, 13.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.9f, 0.1f, 5.5f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.1f);
    ourShader.setVec4("material.diffuse", color * 0.1f);
    ourShader.setVec4("material.specular", color * 0.5f * 0.1f);
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    
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

void LampPost(Shader ourShader, glm::mat4 moveMatrix)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix;

    //base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.01f, -0.42f, 15.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 0.3f, 0.7f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    //pillar
    translateMatrix = glm::translate(identityMatrix, glm::vec3(8.17f, -0.42f, 15.15f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.5f, 0.1f));
    model = translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    ourShader.setVec4("material.diffuse", glm::vec4(0.3f, 0.4f, 0.3f, 1.0f));
    ourShader.setVec4("material.specular", glm::vec4(0.3f, 0.4f, 0.3f, 1.0f));
    ourShader.setFloat("material.shininess", 32.0f);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

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
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    glBindTexture(GL_TEXTURE_2D, texture0);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.0f, 4.0f));
    Table(ourShader, moveMatrix * translateMatrix, 90.0);

    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.70f, 0.0f, 4.75f));
    Chair(ourShader, moveMatrix * translateMatrix, 0);

    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.25f, 0.0f, 4.15f));
    Chair(ourShader, moveMatrix * translateMatrix, 90.0f);

    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.73f, 0.0f, 3.55f));
    Chair(ourShader, moveMatrix * translateMatrix, 180.0f);


}
