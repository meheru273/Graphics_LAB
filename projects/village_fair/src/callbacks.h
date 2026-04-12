// This file is #included by main.cpp.
// It is part of the Village-fair project split for readability.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ── Player movement on XZ plane relative to look direction ──
    float playerSpeed = 4.5f * deltaTime;
    float yawRad = glm::radians(g_playerYaw);
    glm::vec3 forward(cos(yawRad), 0.0f, sin(yawRad));
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));

    g_playerMoving = false;
    glm::vec3 moveDir(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { moveDir += forward; g_playerMoving = true; }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { moveDir -= forward; g_playerMoving = true; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { moveDir -= right;   g_playerMoving = true; }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { moveDir += right;   g_playerMoving = true; }

    if (g_playerMoving) {
        moveDir = glm::normalize(moveDir) * playerSpeed;
        g_playerPos += moveDir;
        g_walkTimer += deltaTime * 8.0f;
    }

    // ── Clamp player inside tent when in interior mode ──
    if (g_insideTent) {
        glm::vec2 offset(g_playerPos.x - g_tentCenter.x,
                         g_playerPos.z - g_tentCenter.z);
        float dist = glm::length(offset);
        if (dist > g_tentRadius) {
            offset = glm::normalize(offset) * (g_tentRadius - 0.05f);
            g_playerPos.x = g_tentCenter.x + offset.x;
            g_playerPos.z = g_tentCenter.z + offset.y;
        }
    }

    // ── Clamp player to extended boundary ──
    if (!g_insideTent) {
        const float X_MIN = -30.0f, X_MAX = 28.0f;
        const float Z_MIN = -29.0f, Z_MAX = 25.0f;
        if (g_playerPos.x < X_MIN) g_playerPos.x = X_MIN;
        if (g_playerPos.x > X_MAX) g_playerPos.x = X_MAX;
        if (g_playerPos.z < Z_MIN) g_playerPos.z = Z_MIN;
        if (g_playerPos.z > Z_MAX) g_playerPos.z = Z_MAX;
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Left Alt — toggle cursor lock (so user can resize window / interact with UI)
    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
    {
        g_cursorLocked = !g_cursorLocked;
        glfwSetInputMode(window, GLFW_CURSOR,
                         g_cursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (g_cursorLocked) firstMouse = true;  // avoid camera jump on re-lock
    }

    if (key == GLFW_KEY_G && action == GLFW_PRESS)                   //Gate Open/Close
    {
        isGateOpening *= -1.0f;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS)                   //Circus tent door open/close
    {
        circusTent.toggleDoor();
    }

    // Q advances the sun one step. 10 presses = full cycle
    // (8 day steps: sunrise->noon->sunset, then 2 night steps).
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        g_sunStep = (g_sunStep + 1) % 10;
    }

    // T — toggle circus tent interior / exterior view
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        g_insideTent = !g_insideTent;
        if (g_insideTent) {
            // Save current player state
            g_savedCamPos = g_playerPos;
            g_savedYaw    = g_playerYaw;
            g_savedPitch  = g_playerPitch;
            // Teleport player inside tent entrance
            g_playerPos   = glm::vec3(-5.0f, -0.42f, -3.0f + 2.5f);
            g_playerYaw   = -90.0f;
            g_playerPitch = -8.0f;
        } else {
            // Restore saved player state
            g_playerPos   = g_savedCamPos;
            g_playerYaw   = g_savedYaw;
            g_playerPitch = g_savedPitch;
        }
    }


    //***************Lighting***************

    if ((key == GLFW_KEY_L && action == GLFW_PRESS))                   //Lighting On/Off
    {
        lightingOn ^= true;
    }
    if ((key == GLFW_KEY_0 && action == GLFW_PRESS))                   //Dark On/Off
    {
        dark ^= true;
        if (dark)
        {
            pointLightOn[0] = 1.0;
            pointLight1.turnOn();
            pointLightOn[1] = 1.0;
            pointLight2.turnOn();
            pointLightOn[2] = 1.0;
            pointLight3.turnOn();
            pointLightOn[3] = 1.0;
            pointLight4.turnOn();
            directionalLightOn = 0.0;
            directionalLight.turnOff();
        }
        else
        {
            pointLightOn[0] = 0.0;
            pointLight1.turnOff();
            pointLightOn[1] = 0.0;
            pointLight2.turnOff();
            pointLightOn[2] = 0.0;
            pointLight3.turnOff();
            pointLightOn[3] = 0.0;
            pointLight4.turnOff();
            directionalLightOn = 1.0;
            directionalLight.turnOn();
        }
    }

    if (key == GLFW_KEY_7 && action == GLFW_PRESS)          //Ambient On/Off
    {
        if (ambientOn == 0.0)                   
        {
            ambientOn = 1.0;
            pointLight1.turnAmbientOn();
            pointLight2.turnAmbientOn();
            pointLight3.turnAmbientOn();
            pointLight4.turnAmbientOn();
            spotLight.turnAmbientOn();
            directionalLight.turnAmbientOn();
        }
        else
        {
            ambientOn = 0.0;
            pointLight1.turnAmbientOff();
            pointLight2.turnAmbientOff();
            pointLight3.turnAmbientOff();
            pointLight4.turnAmbientOff();
            spotLight.turnAmbientOff();
            directionalLight.turnAmbientOff();
        }
    }

    if (key == GLFW_KEY_8 && action == GLFW_PRESS)          //Diffuse On/Off
    {
        if (diffuseOn == 0.0)
        {
            diffuseOn = 1.0;
            pointLight1.turnDiffuseOn();
            pointLight2.turnDiffuseOn();
            pointLight3.turnDiffuseOn();
            pointLight4.turnDiffuseOn();
            spotLight.turnDiffuseOn();
            directionalLight.turnDiffuseOn();
        }
        else
        {
            diffuseOn = 0.0;
            pointLight1.turnDiffuseOff();
            pointLight2.turnDiffuseOff();
            pointLight3.turnDiffuseOff();
            pointLight4.turnDiffuseOff();
            spotLight.turnDiffuseOff();
            directionalLight.turnDiffuseOff();
        }
    }

    if (key == GLFW_KEY_9 && action == GLFW_PRESS)          //Specular On/Off
    {
        if (specularOn == 0.0)
        {
            specularOn = 1.0;
            pointLight1.turnSpecularOn();
            pointLight2.turnSpecularOn();
            pointLight3.turnSpecularOn();
            pointLight4.turnSpecularOn();
            spotLight.turnSpecularOn();
            directionalLight.turnSpecularOn();
        }
        else
        {
            specularOn = 0.0;
            pointLight1.turnSpecularOff();
            pointLight2.turnSpecularOff();
            pointLight3.turnSpecularOff();
            pointLight4.turnSpecularOff();
            spotLight.turnSpecularOff();
            directionalLight.turnSpecularOff();
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)          //Point Light 1 On/Off
    {
        if (pointLightOn[0] == 0.0)
        {
            pointLightOn[0] = 1.0;
            pointLight1.turnOn();
        }
        else
        {
            pointLightOn[0] = 0.0;
            pointLight1.turnOff();
        }
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS)          //Point Light 2 On/Off
    {
        if (pointLightOn[1] == 0.0)
        {
            pointLightOn[1] = 1.0;
            pointLight2.turnOn();
        }
        else
        {
            pointLightOn[1] = 0.0;
            pointLight2.turnOff();
        }
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS)          //Point Light 3 On/Off
    {
        if (pointLightOn[2] == 0.0)
        {
            pointLightOn[2] = 1.0;
            pointLight3.turnOn();
        }
        else
        {
            pointLightOn[2] = 0.0;
            pointLight3.turnOff();
        }
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)          //Point Light 4 On/Off
    {
        if (pointLightOn[3] == 0.0)
        {
            pointLightOn[3] = 1.0;
            pointLight4.turnOn();
        }
        else
        {
            pointLightOn[3] = 0.0;
            pointLight4.turnOff();
        }
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS)          //Directional Light 1 On/Off
    {
        if (directionalLightOn == 0.0)
        {
            directionalLightOn = 1.0;
            directionalLight.turnOn();
        }
        else
        {
            directionalLightOn = 0.0;
            directionalLight.turnOff();
        }
    }

    if (key == GLFW_KEY_6 && action == GLFW_PRESS)          //Spot Light 1 On/Off
    {
        if (spotLightOn == 0.0)
        {
            spotLightOn = 1.0;
            spotLight.turnOn();
        }
        else
        {
            spotLightOn = 0.0;
            spotLight.turnOff();
        }
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)                   //Ferris Wheel On/Off
    {
        isFerrisWheelOn ^= true;
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS)                   //Toggle FPV / Chase camera
    {
        g_camMode = (g_camMode == CAM_FIRST_PERSON) ? CAM_CHASE : CAM_FIRST_PERSON;
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)                   //Boat On/Off
    {
        isBoatOn ^= true;
    }


    if (key == GLFW_KEY_J && action == GLFW_PRESS)                   //Carousel On/Off
    {
        isCarousalOn ^= true;
    }
    if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)                   //Rotation testing
    {
        rotateAngleTest_Y += speed;
        rotateAngleTest_Y = fmod(rotateAngleTest_Y, 360);
        /*cout << rotateAngleTest_Y << endl;*/
    }
    if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)                   //Rotation testing
    {
        rotateAngleTest_Y -= speed;
        rotateAngleTest_Y = fmod(rotateAngleTest_Y, 360);
        /*cout << rotateAngleTest_Y << endl;*/
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
// Left mouse click → toggle all circus spotlights moving
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        circusTent.toggleSpotlights();
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Skip mouse look when cursor is unlocked (user is interacting with window)
    if (!g_cursorLocked) return;

    // Mouse controls look — update player yaw & pitch
    float sensitivity = 0.1f;
    g_playerYaw   += xoffset * sensitivity;
    g_playerPitch += yoffset * sensitivity;

    // Constrain pitch so screen doesn't flip
    if (g_playerPitch >  89.0f) g_playerPitch =  89.0f;
    if (g_playerPitch < -89.0f) g_playerPitch = -89.0f;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Zoom in/out by adjusting FOV
    g_fov -= (float)yoffset * 2.0f;
    if (g_fov < 30.0f) g_fov = 30.0f;
    if (g_fov > 110.0f) g_fov = 110.0f;
}


//Texture Loading