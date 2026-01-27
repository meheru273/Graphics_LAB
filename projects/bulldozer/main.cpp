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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
string readShaderFile(const char* filePath);
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);

// Settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
float bulldozerX = 0.0f;
float bulldozerY = 0.0f;
float bulldozerRotation = 0.0f;
float bulldozerScale = 1.0f;

float wheelRotation = 0.0f;

float bladeOffset = 0.0f;
bool bladeUp = false;

int main()
{
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
        "Interactive 2D Bulldozer - Computer Graphics Lab", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // shader program 
    unsigned int shaderProgram = createShaderProgram(
        "../projects/bulldozer/shaders/vertexShader.vs",
        "../projects/bulldozer/shaders/fragmentShader.fs"
    );
    if (shaderProgram == 0) {
        cout << "CRITICAL: Shader program creation failed! Colors won't display." << endl;
    } else {
        cout << "Shader program created successfully. ID: " << shaderProgram << endl;
    }

    // Main Body 
    float mainBody[] = {
        // Triangle 1
        -0.4f, -0.1f, 0.0f,  
         0.2f, -0.1f, 0.0f,  
         0.2f,  0.25f, 0.0f, 
        // Triangle 2
        -0.4f, -0.1f, 0.0f,  
         0.2f,  0.25f, 0.0f, 
        -0.4f,  0.25f, 0.0f  
    };

    float cabin[] = {
         0.2f, -0.1f, 0.0f,  
         0.6f, -0.1f, 0.0f,  
         0.55f, 0.6f, 0.0f,  
         0.2f, -0.1f, 0.0f,  
         0.55f, 0.6f, 0.0f,  
         0.15f, 0.6f, 0.0f   
    };

    float cabinWindow[] = {
         0.22f,  0.15f, 0.0f,
         0.53f,  0.15f, 0.0f,
         0.50f,  0.55f, 0.0f,
       
         0.22f,  0.15f, 0.0f,
         0.50f,  0.55f, 0.0f,
         0.22f,  0.55f, 0.0f
    };

    float exhaust[] = {
         0.55f,  0.3f, 0.0f,
         0.65f,  0.3f, 0.0f,
         0.65f,  0.5f, 0.0f,
       
         0.55f,  0.3f, 0.0f,
         0.65f,  0.5f, 0.0f,
         0.60f,  0.5f, 0.0f
    };

    // Blade 
    float blade[] = {
        
        -0.65f, -0.4f, 0.0f,  
        -0.50f, -0.4f, 0.0f,  
        -0.45f,  0.2f, 0.0f,  
        
        -0.65f, -0.4f, 0.0f,  
        -0.45f,  0.2f, 0.0f,  
        -0.55f,  0.2f, 0.0f   
    };

   
    float bladeArm[] = {
        -0.4f,  0.0f,  0.0f,
        -0.5f, -0.1f, 0.0f,
        -0.5f, -0.05f,0.0f,
        
        -0.4f,  0.0f,  0.0f,
        -0.5f, -0.05f,0.0f,
        -0.4f,  0.05f, 0.0f
    };

    
    vector<float> rearWheelVertices;
    int numSegments = 30;
    float rearWheelRadius = 0.3f;
    float rearWheelCenterX = 0.45f;
    float rearWheelCenterY = -0.15f;

    // Create wheel as triangle fan 
    for (int i = 0; i < numSegments; i++) {
        float angle1 = 2.0f * 3.14159f * i / numSegments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / numSegments;
        
        // Center point
        rearWheelVertices.push_back(rearWheelCenterX);
        rearWheelVertices.push_back(rearWheelCenterY);
        rearWheelVertices.push_back(0.0f);
        
        // First edge point
        rearWheelVertices.push_back(rearWheelCenterX + rearWheelRadius * cos(angle1));
        rearWheelVertices.push_back(rearWheelCenterY + rearWheelRadius * sin(angle1));
        rearWheelVertices.push_back(0.0f);
        
        // Second edge point
        rearWheelVertices.push_back(rearWheelCenterX + rearWheelRadius * cos(angle2));
        rearWheelVertices.push_back(rearWheelCenterY + rearWheelRadius * sin(angle2));
        rearWheelVertices.push_back(0.0f);
    }

    float rearWheelSpoke[] = {
        rearWheelCenterX, rearWheelCenterY - 0.01f, 0.0f,
        rearWheelCenterX + rearWheelRadius, rearWheelCenterY - 0.01f, 0.0f,
        rearWheelCenterX + rearWheelRadius, rearWheelCenterY + 0.01f, 0.0f,
        
        rearWheelCenterX, rearWheelCenterY - 0.01f, 0.0f,
        rearWheelCenterX + rearWheelRadius, rearWheelCenterY + 0.01f, 0.0f,
        rearWheelCenterX, rearWheelCenterY + 0.01f, 0.0f
    };

    vector<float> frontWheelVertices;
    float frontWheelRadius = 0.2f;
    float frontWheelCenterX = -0.25f;
    float frontWheelCenterY = -0.2f;

    for (int i = 0; i < numSegments; i++) {
        float angle1 = 2.0f * 3.14159f * i / numSegments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / numSegments;
        
        // Center point
        frontWheelVertices.push_back(frontWheelCenterX);
        frontWheelVertices.push_back(frontWheelCenterY);
        frontWheelVertices.push_back(0.0f);
        
        // First edge point
        frontWheelVertices.push_back(frontWheelCenterX + frontWheelRadius * cos(angle1));
        frontWheelVertices.push_back(frontWheelCenterY + frontWheelRadius * sin(angle1));
        frontWheelVertices.push_back(0.0f);
        
        // Second edge point
        frontWheelVertices.push_back(frontWheelCenterX + frontWheelRadius * cos(angle2));
        frontWheelVertices.push_back(frontWheelCenterY + frontWheelRadius * sin(angle2));
        frontWheelVertices.push_back(0.0f);
    }

    // Front wheel spoke
    float frontWheelSpoke[] = {
        frontWheelCenterX, frontWheelCenterY - 0.01f, 0.0f,
        frontWheelCenterX + frontWheelRadius, frontWheelCenterY - 0.01f, 0.0f,
        frontWheelCenterX + frontWheelRadius, frontWheelCenterY + 0.01f, 0.0f,
        
        frontWheelCenterX, frontWheelCenterY - 0.01f, 0.0f,
        frontWheelCenterX + frontWheelRadius, frontWheelCenterY + 0.01f, 0.0f,
        frontWheelCenterX, frontWheelCenterY + 0.01f, 0.0f
    };

    //  VAOs and VBOs
    unsigned int VBO_body, VAO_body;
    glGenVertexArrays(1, &VAO_body);
    glGenBuffers(1, &VBO_body);
    glBindVertexArray(VAO_body);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_body);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mainBody), mainBody, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_cabin, VAO_cabin;
    glGenVertexArrays(1, &VAO_cabin);
    glGenBuffers(1, &VBO_cabin);
    glBindVertexArray(VAO_cabin);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cabin);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cabin), cabin, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_window, VAO_window;
    glGenVertexArrays(1, &VAO_window);
    glGenBuffers(1, &VBO_window);
    glBindVertexArray(VAO_window);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_window);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cabinWindow), cabinWindow, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_exhaust, VAO_exhaust;
    glGenVertexArrays(1, &VAO_exhaust);
    glGenBuffers(1, &VBO_exhaust);
    glBindVertexArray(VAO_exhaust);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_exhaust);
    glBufferData(GL_ARRAY_BUFFER, sizeof(exhaust), exhaust, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_blade, VAO_blade;
    glGenVertexArrays(1, &VAO_blade);
    glGenBuffers(1, &VBO_blade);
    glBindVertexArray(VAO_blade);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_blade);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blade), blade, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_arm, VAO_arm;
    glGenVertexArrays(1, &VAO_arm);
    glGenBuffers(1, &VBO_arm);
    glBindVertexArray(VAO_arm);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_arm);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bladeArm), bladeArm, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_rearWheel, VAO_rearWheel;
    glGenVertexArrays(1, &VAO_rearWheel);
    glGenBuffers(1, &VBO_rearWheel);
    glBindVertexArray(VAO_rearWheel);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_rearWheel);
    glBufferData(GL_ARRAY_BUFFER, rearWheelVertices.size() * sizeof(float), 
                 &rearWheelVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_frontWheel, VAO_frontWheel;
    glGenVertexArrays(1, &VAO_frontWheel);
    glGenBuffers(1, &VBO_frontWheel);
    glBindVertexArray(VAO_frontWheel);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_frontWheel);
    glBufferData(GL_ARRAY_BUFFER, frontWheelVertices.size() * sizeof(float), 
                 &frontWheelVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_rearSpoke, VAO_rearSpoke;
    glGenVertexArrays(1, &VAO_rearSpoke);
    glGenBuffers(1, &VBO_rearSpoke);
    glBindVertexArray(VAO_rearSpoke);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_rearSpoke);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rearWheelSpoke), rearWheelSpoke, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO_frontSpoke, VAO_frontSpoke;
    glGenVertexArrays(1, &VAO_frontSpoke);
    glGenBuffers(1, &VBO_frontSpoke);
    glBindVertexArray(VAO_frontSpoke);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_frontSpoke);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frontWheelSpoke), frontWheelSpoke, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Print controls
    cout << "\n=== BULLDOZER CONTROLS ===" << endl;
    cout << "ENTIRE BULLDOZER:" << endl;
    cout << "  W/S/A/D - Move bulldozer" << endl;
    cout << "  Q/E - Rotate bulldozer" << endl;
    cout << "  Z/X - Scale bulldozer" << endl;
    cout << "\nINDIVIDUAL PARTS:" << endl;
    cout << "  R - Rotate wheels clockwise" << endl;
    cout << "  T - Rotate wheels counter-clockwise" << endl;
    cout << "  B - Raise/Lower blade" << endl;
    cout << "\nESC - Exit" << endl;
    cout << "\n*** ALL SHAPES DRAWN USING TRIANGLES (GL_TRIANGLES) ***" << endl;
    cout << "*** USING TRANSFORMATION MATRICES ***\n" << endl;

    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
        if (colorLoc == -1) {
            cout << "ERROR: objectColor uniform not found in shader! Colors will not work." << endl;
        } 

        // transformation matrices for entire bulldozer
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), 
                                                     glm::vec3(bulldozerX, bulldozerY, 0.0f));
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), 
                                              glm::radians(bulldozerRotation), 
                                              glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), 
                                          glm::vec3(bulldozerScale, bulldozerScale, 1.0f));
        
        glm::mat4 bulldozerTransform = translationMatrix * rotationMatrix * scaleMatrix;

        //DRAW WHEELS (BACK LAYER)
        glm::mat4 rearWheelTranslate1 = glm::translate(glm::mat4(1.0f), 
                                                       glm::vec3(rearWheelCenterX, rearWheelCenterY, 0.0f));
        glm::mat4 rearWheelRotate = glm::rotate(glm::mat4(1.0f), 
                                               glm::radians(wheelRotation), 
                                               glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rearWheelTranslate2 = glm::translate(glm::mat4(1.0f), 
                                                       glm::vec3(-rearWheelCenterX, -rearWheelCenterY, 0.0f));
        
        glm::mat4 rearWheelTransform = bulldozerTransform * rearWheelTranslate1 * 
                                       rearWheelRotate * rearWheelTranslate2;

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(rearWheelTransform));
        glUniform4f(colorLoc, 0.15f, 0.15f, 0.15f, 1.0f);
        glBindVertexArray(VAO_rearWheel);
        glDrawArrays(GL_TRIANGLES, 0, numSegments * 3);

        // Rear wheel spoke
        glUniform4f(colorLoc, 0.9f, 0.9f, 0.9f, 1.0f);
        glBindVertexArray(VAO_rearSpoke);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Front Wheel
        glm::mat4 frontWheelTranslate1 = glm::translate(glm::mat4(1.0f), 
                                                        glm::vec3(frontWheelCenterX, frontWheelCenterY, 0.0f));
        glm::mat4 frontWheelRotate = glm::rotate(glm::mat4(1.0f), 
                                                glm::radians(wheelRotation), 
                                                glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 frontWheelTranslate2 = glm::translate(glm::mat4(1.0f), 
                                                        glm::vec3(-frontWheelCenterX, -frontWheelCenterY, 0.0f));
        
        glm::mat4 frontWheelTransform = bulldozerTransform * frontWheelTranslate1 * 
                                        frontWheelRotate * frontWheelTranslate2;

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(frontWheelTransform));
        glUniform4f(colorLoc, 0.15f, 0.15f, 0.15f, 1.0f);
        glBindVertexArray(VAO_frontWheel);
        glDrawArrays(GL_TRIANGLES, 0, numSegments * 3);

        glUniform4f(colorLoc, 0.9f, 0.9f, 0.9f, 1.0f);
        glBindVertexArray(VAO_frontSpoke);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //DRAW BODy
        // Main body
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(bulldozerTransform));
        glUniform4f(colorLoc, 0.95f, 0.8f, 0.1f, 1.0f);
        glBindVertexArray(VAO_body);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Cabin
        glUniform4f(colorLoc, 0.95f, 0.8f, 0.1f, 1.0f);
        glBindVertexArray(VAO_cabin);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Window
        glUniform4f(colorLoc, 0.6f, 0.8f, 0.95f, 1.0f);
        glBindVertexArray(VAO_window);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Exhaust
        glUniform4f(colorLoc, 0.3f, 0.3f, 0.3f, 1.0f);
        glBindVertexArray(VAO_exhaust);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glm::mat4 bladeTranslate = glm::translate(glm::mat4(1.0f), 
                                                  glm::vec3(0.0f, bladeOffset, 0.0f));
        glm::mat4 bladeTransform = bulldozerTransform * bladeTranslate;

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(bladeTransform));
        glUniform4f(colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
        glBindVertexArray(VAO_blade);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Blade arm
        glUniform4f(colorLoc, 0.2f, 0.2f, 0.2f, 1.0f);
        glBindVertexArray(VAO_arm);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO_body);
    glDeleteBuffers(1, &VBO_body);
    glDeleteVertexArrays(1, &VAO_cabin);
    glDeleteBuffers(1, &VBO_cabin);
    glDeleteVertexArrays(1, &VAO_window);
    glDeleteBuffers(1, &VBO_window);
    glDeleteVertexArrays(1, &VAO_exhaust);
    glDeleteBuffers(1, &VBO_exhaust);
    glDeleteVertexArrays(1, &VAO_blade);
    glDeleteBuffers(1, &VBO_blade);
    glDeleteVertexArrays(1, &VAO_arm);
    glDeleteBuffers(1, &VBO_arm);
    glDeleteVertexArrays(1, &VAO_rearWheel);
    glDeleteBuffers(1, &VBO_rearWheel);
    glDeleteVertexArrays(1, &VAO_frontWheel);
    glDeleteBuffers(1, &VBO_frontWheel);
    glDeleteVertexArrays(1, &VAO_rearSpoke);
    glDeleteBuffers(1, &VBO_rearSpoke);
    glDeleteVertexArrays(1, &VAO_frontSpoke);
    glDeleteBuffers(1, &VBO_frontSpoke);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        bulldozerY += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        bulldozerY -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        bulldozerX -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        bulldozerX += 0.01f;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        bulldozerRotation += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        bulldozerRotation -= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        bulldozerScale += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        bulldozerScale = max(0.1f, bulldozerScale - 0.01f);


    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        wheelRotation += 3.0f;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        wheelRotation -= 3.0f;

    
    static bool bKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bKeyPressed)
    {
        bladeUp = !bladeUp;
        bKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        bKeyPressed = false;
    }

    if (bladeUp && bladeOffset < 0.2f)
        bladeOffset += 0.005f;
    if (!bladeUp && bladeOffset > 0.0f)
        bladeOffset -= 0.005f;
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
        cerr << "Make sure you're running from the correct directory or adjust the shader path." << endl;
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
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}