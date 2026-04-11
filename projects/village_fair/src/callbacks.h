// This file is #included by main.cpp.
// It is part of the Village-fair project split for readability.
// All global variables and #includes are inherited from main.cpp.
#pragma once

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ── Movement (always allowed) ──
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    // ── Controls that only work OUTSIDE the tent ──
    if (!g_insideTent) {
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            camera.ProcessYPR(0.0f, 3.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            camera.ProcessYPR(0.0f, -3.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
            camera.ProcessYPR(3.0f, 0.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            camera.ProcessYPR(-3.0f, 0.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            camera.ProcessYPR(0.0f, 0.0f, 0.5f);
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
            camera.RotateAroundLookAt(2.0f);
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
            camera.RotateAroundLookAt(-2.0f);
    }

    // ── Clamp camera inside tent when in interior mode ──
    if (g_insideTent) {
        // Clamp XZ to tent cylinder (strict — no escaping)
        glm::vec2 offset(camera.Position.x - g_tentCenter.x,
                         camera.Position.z - g_tentCenter.z);
        float dist = glm::length(offset);
        if (dist > g_tentRadius) {
            offset = glm::normalize(offset) * (g_tentRadius - 0.05f);
            camera.Position.x = g_tentCenter.x + offset.x;
            camera.Position.z = g_tentCenter.z + offset.y;
        }
        // Clamp Y between floor and ceiling
        if (camera.Position.y < g_tentFloorY)
            camera.Position.y = g_tentFloorY;
        if (camera.Position.y > g_tentCeilY)
            camera.Position.y = g_tentCeilY;
        camera.LOOKAT = camera.Position + camera.Front;
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
            // Save current camera state
            g_savedCamPos = camera.Position;
            g_savedYaw    = camera.Yaw;
            g_savedPitch  = camera.Pitch;
            // Tent centre=(-5,-0.42,-3), entrance gap on +Z side
            // Spawn just inside the entrance, look toward -Z (into tent interior)
            camera.Position = glm::vec3(-5.0f, -0.42f + 1.6f, -3.0f + 2.5f);
            camera.Yaw   = -90.0f;  // -90° → front=(0,0,-1) → looking into tent
            camera.Pitch =  -8.0f;
            camera.updateCameraVectors();
        } else {
            // Restore saved camera
            camera.Position = g_savedCamPos;
            camera.Yaw      = g_savedYaw;
            camera.Pitch    = g_savedPitch;
            camera.updateCameraVectors();
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
    if (key == GLFW_KEY_V && action == GLFW_PRESS)                   //Ferris Wheel On/Off
    {
        isFerrisWheelOn ^= true;
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

    // Only rotate the camera while the right mouse button is held down.
    // This frees the cursor for window interaction (resize, focus, etc.).
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
        return;
    }

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


//Texture Loading