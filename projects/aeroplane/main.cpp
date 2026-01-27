#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
string readShaderFile(const char* filePath);
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);
void drawCube(unsigned int VAO, glm::mat4 model, unsigned int shaderProgram, glm::vec4 color);
void drawCylinder(unsigned int VAO, int segments, glm::mat4 model, unsigned int shaderProgram, glm::vec4 color);

// Settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;
float cameraFov = 45.0f;

// Mouse control
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Airplane controls
glm::vec3 planePos = glm::vec3(0.0f, 0.0f, 0.0f);
float planePitch = 0.0f;   // X-axis rotation
float planeYaw = 0.0f;     // Y-axis rotation
float planeRoll = 0.0f;    // Z-axis rotation
float planeSpeed = 0.0f;
float planeAltitude = 0.0f;

// Airplane parts animation
float propellerRotation = 0.0f;
bool engineRunning = false;
float doorAngle = 0.0f;
bool doorOpen = false;
float windowShade = 0.0f;
bool windowOpen = false;

// Light
bool lightOn = true;
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

// Camera mode
enum CameraMode { FREE_CAM, BIRD_EYE, FOLLOW_PLANE };
CameraMode cameraMode = FOLLOW_PLANE;

// Look-at point for orbit camera
glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 0.0f);
float orbitRadius = 15.0f;
float orbitAngle = 0.0f;

// Flight dynamics
bool takingOff = false;
bool landing = false;
float takeoffProgress = 0.0f;
float landingProgress = 0.0f;

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
        "3D Airplane Simulator - Computer Graphics Assignment", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Create shader program
    unsigned int shaderProgram = createShaderProgram(
        "../projects/aeroplane/shaders/vertexShader.vs",
        "../projects/aeroplane/shaders/fragmentShader.fs"
    );

    if (shaderProgram == 0) {
        cout << "CRITICAL: Shader program creation failed!" << endl;
        return -1;
    }

    // Create cube VAO for fuselage, wings, tail
    float cubeVertices[] = {
        // Positions (simple cube)
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create cylinder VAO for engines and propeller shaft
    vector<float> cylinderVertices;
    int cylinderSegments = 30;
    float cylinderRadius = 0.5f;
    float cylinderHeight = 1.0f;

    for (int i = 0; i < cylinderSegments; i++) {
        float angle1 = 2.0f * 3.14159f * i / cylinderSegments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / cylinderSegments;
        
        // Side triangles
        cylinderVertices.push_back(cos(angle1) * cylinderRadius);
        cylinderVertices.push_back(-cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle1) * cylinderRadius);
        
        cylinderVertices.push_back(cos(angle2) * cylinderRadius);
        cylinderVertices.push_back(-cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle2) * cylinderRadius);
        
        cylinderVertices.push_back(cos(angle2) * cylinderRadius);
        cylinderVertices.push_back(cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle2) * cylinderRadius);
        
        cylinderVertices.push_back(cos(angle1) * cylinderRadius);
        cylinderVertices.push_back(-cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle1) * cylinderRadius);
        
        cylinderVertices.push_back(cos(angle2) * cylinderRadius);
        cylinderVertices.push_back(cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle2) * cylinderRadius);
        
        cylinderVertices.push_back(cos(angle1) * cylinderRadius);
        cylinderVertices.push_back(cylinderHeight / 2);
        cylinderVertices.push_back(sin(angle1) * cylinderRadius);
    }

    unsigned int cylinderVAO, cylinderVBO;
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    glBindVertexArray(cylinderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), &cylinderVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Print controls
    cout << "\n========== 3D AIRPLANE SIMULATOR CONTROLS ==========" << endl;
    cout << "\n=== AIRPLANE CONTROLS ===" << endl;
    cout << "  W - Move forward (throttle up)" << endl;
    cout << "  S - Move backward (throttle down)" << endl;
    cout << "  A - Move left" << endl;
    cout << "  D - Move right" << endl;
    cout << "  E - Move up (increase altitude)" << endl;
    cout << "  R - Move down (decrease altitude)" << endl;
    cout << "  X - Pitch (nose up/down)" << endl;
    cout << "  Y - Yaw (turn left/right)" << endl;
    cout << "  Z - Roll (bank left/right)" << endl;
    cout << "\n=== AIRPLANE FEATURES ===" << endl;
    cout << "  G - Toggle engine (propeller rotation)" << endl;
    cout << "  L - Toggle lights" << endl;
    cout << "  O - Open/Close door" << endl;
    cout << "  P - Open/Close window shades" << endl;
    cout << "  T - Takeoff sequence" << endl;
    cout << "  U - Landing sequence" << endl;
    cout << "\n=== CAMERA CONTROLS ===" << endl;
    cout << "  F - Orbit camera around plane" << endl;
    cout << "  C - Switch camera mode (Follow/Bird Eye/Free)" << endl;
    cout << "  Mouse - Look around (in Free Cam mode)" << endl;
    cout << "  Scroll - Zoom in/out" << endl;
    cout << "\n  ESC - Exit" << endl;
    cout << "===================================================\n" << endl;

    // Main render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Update propeller rotation
        if (engineRunning) {
            propellerRotation += 1000.0f * deltaTime;
            if (propellerRotation > 360.0f) propellerRotation -= 360.0f;
        }

        // Update takeoff/landing
        if (takingOff) {
            takeoffProgress += deltaTime * 0.3f;
            if (takeoffProgress >= 1.0f) {
                takeoffProgress = 1.0f;
                takingOff = false;
            }
            planePos.z -= 15.0f * deltaTime;
            planePos.y = takeoffProgress * 8.0f;
            planePitch = -takeoffProgress * 15.0f;
        }

        if (landing) {
            landingProgress += deltaTime * 0.3f;
            if (landingProgress >= 1.0f) {
                landingProgress = 1.0f;
                landing = false;
                planePos.y = 0.0f;
            }
            planePos.z += 15.0f * deltaTime;
            planePos.y = (1.0f - landingProgress) * 8.0f;
            planePitch = (1.0f - landingProgress) * 15.0f;
        }

        // Clear buffers
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f); // Sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader
        glUseProgram(shaderProgram);

        // Setup camera based on mode
        glm::mat4 view;
        if (cameraMode == BIRD_EYE) {
            view = glm::lookAt(planePos + glm::vec3(0.0f, 20.0f, 0.1f),
                              planePos,
                              glm::vec3(0.0f, 0.0f, -1.0f));
        } else if (cameraMode == FOLLOW_PLANE) {
            glm::vec3 offset = glm::vec3(0.0f, 3.0f, 10.0f);
            glm::mat4 planeRotation = glm::rotate(glm::mat4(1.0f), glm::radians(planeYaw), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 rotatedOffset = glm::vec3(planeRotation * glm::vec4(offset, 1.0f));
            view = glm::lookAt(planePos + rotatedOffset, planePos, glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        }

        glm::mat4 projection = glm::perspective(glm::radians(cameraFov), 
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                0.1f, 100.0f);

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Set light uniform
        unsigned int useLightLoc = glGetUniformLocation(shaderProgram, "useLight");
        unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform1i(useLightLoc, lightOn);
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        // ===== DRAW AIRPLANE =====
        
        // Base transformation for entire plane
        glm::mat4 planeTransform = glm::mat4(1.0f);
        planeTransform = glm::translate(planeTransform, planePos);
        planeTransform = glm::rotate(planeTransform, glm::radians(planeYaw), glm::vec3(0.0f, 1.0f, 0.0f));
        planeTransform = glm::rotate(planeTransform, glm::radians(planePitch), glm::vec3(1.0f, 0.0f, 0.0f));
        planeTransform = glm::rotate(planeTransform, glm::radians(planeRoll), glm::vec3(0.0f, 0.0f, 1.0f));

        // Fuselage (main body)
        glm::mat4 fuselageModel = planeTransform;
        fuselageModel = glm::scale(fuselageModel, glm::vec3(1.5f, 1.2f, 6.0f));
        drawCube(cubeVAO, fuselageModel, shaderProgram, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

        // Cockpit
        glm::mat4 cockpitModel = planeTransform;
        cockpitModel = glm::translate(cockpitModel, glm::vec3(0.0f, 0.8f, -2.5f));
        cockpitModel = glm::scale(cockpitModel, glm::vec3(1.3f, 0.8f, 1.5f));
        drawCube(cubeVAO, cockpitModel, shaderProgram, glm::vec4(0.3f, 0.5f, 0.8f, 0.7f));

        // Wings (left and right)
        glm::mat4 leftWingModel = planeTransform;
        leftWingModel = glm::translate(leftWingModel, glm::vec3(-3.5f, 0.0f, 0.0f));
        leftWingModel = glm::scale(leftWingModel, glm::vec3(5.0f, 0.2f, 2.5f));
        drawCube(cubeVAO, leftWingModel, shaderProgram, glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));

        glm::mat4 rightWingModel = planeTransform;
        rightWingModel = glm::translate(rightWingModel, glm::vec3(3.5f, 0.0f, 0.0f));
        rightWingModel = glm::scale(rightWingModel, glm::vec3(5.0f, 0.2f, 2.5f));
        drawCube(cubeVAO, rightWingModel, shaderProgram, glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));

        // Engines (under wings)
        glm::mat4 leftEngineModel = planeTransform;
        leftEngineModel = glm::translate(leftEngineModel, glm::vec3(-2.5f, -0.5f, 0.0f));
        leftEngineModel = glm::rotate(leftEngineModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        leftEngineModel = glm::scale(leftEngineModel, glm::vec3(0.7f, 1.5f, 0.7f));
        drawCylinder(cylinderVAO, cylinderSegments, leftEngineModel, shaderProgram, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        glm::mat4 rightEngineModel = planeTransform;
        rightEngineModel = glm::translate(rightEngineModel, glm::vec3(2.5f, -0.5f, 0.0f));
        rightEngineModel = glm::rotate(rightEngineModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rightEngineModel = glm::scale(rightEngineModel, glm::vec3(0.7f, 1.5f, 0.7f));
        drawCylinder(cylinderVAO, cylinderSegments, rightEngineModel, shaderProgram, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        // Propellers (fans)
        glm::mat4 leftPropModel = planeTransform;
        leftPropModel = glm::translate(leftPropModel, glm::vec3(-2.5f, -0.5f, 0.8f));
        leftPropModel = glm::rotate(leftPropModel, glm::radians(propellerRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        leftPropModel = glm::scale(leftPropModel, glm::vec3(2.0f, 0.1f, 0.2f));
        drawCube(cubeVAO, leftPropModel, shaderProgram, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

        glm::mat4 leftProp2Model = planeTransform;
        leftProp2Model = glm::translate(leftProp2Model, glm::vec3(-2.5f, -0.5f, 0.8f));
        leftProp2Model = glm::rotate(leftProp2Model, glm::radians(propellerRotation + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        leftProp2Model = glm::scale(leftProp2Model, glm::vec3(2.0f, 0.1f, 0.2f));
        drawCube(cubeVAO, leftProp2Model, shaderProgram, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

        glm::mat4 rightPropModel = planeTransform;
        rightPropModel = glm::translate(rightPropModel, glm::vec3(2.5f, -0.5f, 0.8f));
        rightPropModel = glm::rotate(rightPropModel, glm::radians(propellerRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        rightPropModel = glm::scale(rightPropModel, glm::vec3(2.0f, 0.1f, 0.2f));
        drawCube(cubeVAO, rightPropModel, shaderProgram, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

        glm::mat4 rightProp2Model = planeTransform;
        rightProp2Model = glm::translate(rightProp2Model, glm::vec3(2.5f, -0.5f, 0.8f));
        rightProp2Model = glm::rotate(rightProp2Model, glm::radians(propellerRotation + 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rightProp2Model = glm::scale(rightProp2Model, glm::vec3(2.0f, 0.1f, 0.2f));
        drawCube(cubeVAO, rightProp2Model, shaderProgram, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

        // Tail (vertical stabilizer)
        glm::mat4 tailVertModel = planeTransform;
        tailVertModel = glm::translate(tailVertModel, glm::vec3(0.0f, 1.2f, 2.8f));
        tailVertModel = glm::scale(tailVertModel, glm::vec3(0.2f, 2.0f, 1.0f));
        drawCube(cubeVAO, tailVertModel, shaderProgram, glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));

        // Horizontal stabilizer
        glm::mat4 tailHorizModel = planeTransform;
        tailHorizModel = glm::translate(tailHorizModel, glm::vec3(0.0f, 1.8f, 2.8f));
        tailHorizModel = glm::scale(tailHorizModel, glm::vec3(2.5f, 0.2f, 1.0f));
        drawCube(cubeVAO, tailHorizModel, shaderProgram, glm::vec4(0.2f, 0.4f, 0.8f, 1.0f));

        // Door
        glm::mat4 doorModel = planeTransform;
        doorModel = glm::translate(doorModel, glm::vec3(-0.8f - doorAngle * 0.5f, -0.2f, 1.0f));
        doorModel = glm::rotate(doorModel, glm::radians(-doorAngle * 60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        doorModel = glm::scale(doorModel, glm::vec3(0.05f, 1.2f, 1.0f));
        drawCube(cubeVAO, doorModel, shaderProgram, glm::vec4(0.5f, 0.3f, 0.1f, 1.0f));

        // Windows (multiple on fuselage)
        for (int i = 0; i < 5; i++) {
            glm::mat4 windowModel = planeTransform;
            windowModel = glm::translate(windowModel, glm::vec3(0.76f, 0.3f, -1.5f + i * 0.8f));
            windowModel = glm::scale(windowModel, glm::vec3(0.02f, 0.4f, 0.5f));
            float windowAlpha = windowOpen ? 0.3f : 0.6f;
            drawCube(cubeVAO, windowModel, shaderProgram, glm::vec4(0.7f, 0.9f, 1.0f, windowAlpha));
        }

        // Landing gear (wheels)
        glm::mat4 frontGearModel = planeTransform;
        frontGearModel = glm::translate(frontGearModel, glm::vec3(0.0f, -1.0f, -2.0f));
        frontGearModel = glm::scale(frontGearModel, glm::vec3(0.2f, 0.6f, 0.2f));
        drawCube(cubeVAO, frontGearModel, shaderProgram, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        glm::mat4 leftGearModel = planeTransform;
        leftGearModel = glm::translate(leftGearModel, glm::vec3(-1.5f, -1.0f, 0.5f));
        leftGearModel = glm::scale(leftGearModel, glm::vec3(0.2f, 0.6f, 0.2f));
        drawCube(cubeVAO, leftGearModel, shaderProgram, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        glm::mat4 rightGearModel = planeTransform;
        rightGearModel = glm::translate(rightGearModel, glm::vec3(1.5f, -1.0f, 0.5f));
        rightGearModel = glm::scale(rightGearModel, glm::vec3(0.2f, 0.6f, 0.2f));
        drawCube(cubeVAO, rightGearModel, shaderProgram, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        // Draw runway/ground
        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, -2.0f, 0.0f));
        groundModel = glm::scale(groundModel, glm::vec3(50.0f, 0.1f, 100.0f));
        drawCube(cubeVAO, groundModel, shaderProgram, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        // Runway markings
        for (int i = -10; i < 10; i++) {
            glm::mat4 lineModel = glm::mat4(1.0f);
            lineModel = glm::translate(lineModel, glm::vec3(0.0f, -1.94f, i * 5.0f));
            lineModel = glm::scale(lineModel, glm::vec3(0.3f, 0.05f, 2.0f));
            drawCube(cubeVAO, lineModel, shaderProgram, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cylinderVAO);
    glDeleteBuffers(1, &cylinderVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void drawCube(unsigned int VAO, glm::mat4 model, unsigned int shaderProgram, glm::vec4 color)
{
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCylinder(unsigned int VAO, int segments, glm::mat4 model, unsigned int shaderProgram, glm::vec4 color)
{
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, segments * 6);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float speed = 5.0f * deltaTime;
    float rotSpeed = 50.0f * deltaTime;

    // Airplane movement (flight simulator style)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Forward - move in the direction the plane is facing
        glm::vec3 forward = glm::vec3(
            sin(glm::radians(planeYaw)),
            0.0f,
            -cos(glm::radians(planeYaw))
        );
        planePos += forward * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Backward
        glm::vec3 forward = glm::vec3(
            sin(glm::radians(planeYaw)),
            0.0f,
            -cos(glm::radians(planeYaw))
        );
        planePos -= forward * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Left
        glm::vec3 right = glm::vec3(
            cos(glm::radians(planeYaw)),
            0.0f,
            sin(glm::radians(planeYaw))
        );
        planePos -= right * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // Right
        glm::vec3 right = glm::vec3(
            cos(glm::radians(planeYaw)),
            0.0f,
            sin(glm::radians(planeYaw))
        );
        planePos += right * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        // Up
        planePos.y += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        // Down
        planePos.y -= speed;
    }

    // Airplane rotations
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        // Pitch
        planePitch += rotSpeed;
        if (planePitch > 360.0f) planePitch -= 360.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        // Yaw
        planeYaw += rotSpeed;
        if (planeYaw > 360.0f) planeYaw -= 360.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        // Roll
        planeRoll += rotSpeed;
        if (planeRoll > 360.0f) planeRoll -= 360.0f;
    }

    // Engine toggle
    static bool gKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyPressed) {
        engineRunning = !engineRunning;
        gKeyPressed = true;
        cout << "Engine " << (engineRunning ? "started" : "stopped") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
        gKeyPressed = false;
    }

    // Light toggle
    static bool lKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyPressed) {
        lightOn = !lightOn;
        lKeyPressed = true;
        cout << "Lights " << (lightOn ? "on" : "off") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lKeyPressed = false;
    }

    // Door control
    static bool oKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed) {
        doorOpen = !doorOpen;
        oKeyPressed = true;
        cout << "Door " << (doorOpen ? "opening" : "closing") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
        oKeyPressed = false;
    }

    // Animate door
    if (doorOpen && doorAngle < 1.0f) {
        doorAngle += deltaTime * 1.0f;
        if (doorAngle > 1.0f) doorAngle = 1.0f;
    } else if (!doorOpen && doorAngle > 0.0f) {
        doorAngle -= deltaTime * 1.0f;
        if (doorAngle < 0.0f) doorAngle = 0.0f;
    }

    // Window control
    static bool pKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pKeyPressed) {
        windowOpen = !windowOpen;
        pKeyPressed = true;
        cout << "Window shades " << (windowOpen ? "opened" : "closed") << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
        pKeyPressed = false;
    }

    // Takeoff sequence
    static bool tKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tKeyPressed) {
        takingOff = true;
        landing = false;
        takeoffProgress = 0.0f;
        engineRunning = true;
        tKeyPressed = true;
        cout << "Initiating takeoff sequence..." << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        tKeyPressed = false;
    }

    // Landing sequence
    static bool uKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !uKeyPressed) {
        landing = true;
        takingOff = false;
        landingProgress = 0.0f;
        uKeyPressed = true;
        cout << "Initiating landing sequence..." << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE) {
        uKeyPressed = false;
    }

    // Orbit camera
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        orbitAngle += rotSpeed * 2.0f;
        if (orbitAngle > 360.0f) orbitAngle -= 360.0f;
        
        cameraPos.x = planePos.x + orbitRadius * cos(glm::radians(orbitAngle));
        cameraPos.z = planePos.z + orbitRadius * sin(glm::radians(orbitAngle));
        cameraPos.y = planePos.y + 5.0f;
        
        cameraFront = glm::normalize(planePos - cameraPos);
        cameraMode = FREE_CAM;
    }

    // Camera mode switch
    static bool cKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
        cameraMode = (CameraMode)((cameraMode + 1) % 3);
        cKeyPressed = true;
        string modeName = (cameraMode == FREE_CAM) ? "Free Camera" : 
                         (cameraMode == BIRD_EYE) ? "Bird's Eye View" : "Follow Plane";
        cout << "Camera mode: " << modeName << endl;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        cKeyPressed = false;
    }

    // Free camera movement (only in free cam mode)
    if (cameraMode == FREE_CAM) {
        float camSpeed = 10.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cameraPos += camSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cameraPos -= camSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camSpeed;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cameraMode != FREE_CAM) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraFov -= (float)yoffset;
    if (cameraFov < 1.0f)
        cameraFov = 1.0f;
    if (cameraFov > 90.0f)
        cameraFov = 90.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

string readShaderFile(const char* filePath)
{
    string content;
    ifstream fileStream(filePath, ios::in);

    if (!fileStream.is_open()) {
        cerr << "ERROR: Could not read shader file: " << filePath << endl;
        cerr << "Make sure you're running from the correct directory." << endl;
        return "";
    }

    string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    cout << "Successfully loaded shader: " << filePath << endl;
    return content;
}

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    string vertexCode = readShaderFile(vertexPath);
    string fragmentCode = readShaderFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        cerr << "ERROR: Shader files could not be loaded!" << endl;
        return 0;
    }

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        return 0;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        return 0;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    cout << "Shader program created successfully!" << endl;
    return shaderProgram;
}
