// Miscellaneous props — #included by main.cpp.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void drawKite(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 barColor, float scale, glm::vec3 position);
void drawKiteWithStick(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 stickColor, float scale, glm::vec3 position);

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

// Function to draw a tree using Bezier curves
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
