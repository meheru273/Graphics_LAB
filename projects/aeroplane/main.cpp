#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// Callbacks & helpers
void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
void mouse_callback(GLFWwindow* w, double xpos, double ypos);
void scroll_callback(GLFWwindow* w, double xo, double yo);
void drawCube(Shader& s, unsigned int VAO, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin);
void drawCylinder(Shader& s, unsigned int VAO, int segs, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin);
void drawSphere(Shader& s, unsigned int VAO, int vertCount, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin);

const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 900;

// Camera
glm::vec3 cameraPos(0.0f, 5.0f, 20.0f), cameraFront(0.0f, 0.0f, -1.0f), cameraUp(0.0f, 1.0f, 0.0f);
float cameraYaw = -90.0f, cameraPitch = 0.0f, cameraFov = 45.0f;
bool firstMouse = true;
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;
float deltaTime = 0.0f, lastFrame = 0.0f;

// Plane state
glm::vec3 planePos(0.0f);
float planePitch=0, planeYaw=0, planeRoll=0;
float propellerRotation = 0.0f;
bool engineRunning = false;
float doorAngle = 0.0f;
bool doorOpen = false, windowOpen = false;

// Lights
bool dirLightOn = true, pointLightOn = true, spotLightOn = true;
bool ambientOn = true, diffuseOn = true, specularOn = true;
bool allLightsOn = true;

// Camera mode
enum CamMode { FREE_CAM, BIRD_EYE, FOLLOW_PLANE, INSIDE_CABIN };
CamMode cameraMode = FOLLOW_PLANE;
float orbitAngle = 0.0f, orbitRadius = 15.0f;

// Flight
bool takingOff = false, landing = false;
float takeoffProgress = 0.0f, landingProgress = 0.0f;

// Viewports
bool splitView = false;

glm::mat4 myLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    // Build camera coordinate frame
    glm::vec3 f = glm::normalize(center - eye);      // Forward (where camera looks)
    glm::vec3 r = glm::normalize(glm::cross(f, up)); // Right (x-axis)
    glm::vec3 u = glm::cross(r, f);                  // Up (y-axis), already normalized
    
    glm::mat4 result(1.0f);
    
    // Rotation part (inverse rotation = transpose)
    result[0][0] = r.x;
    result[1][0] = r.y;
    result[2][0] = r.z;
    
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    
    result[0][2] = -f.x;  // Negate because camera looks down -Z axis in OpenGL
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    
    // Translation part (negative because we're moving the world, not the camera)
    result[3][0] = -glm::dot(r, eye);
    result[3][1] = -glm::dot(u, eye);
    result[3][2] =  glm::dot(f, eye);
    
    return result;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Airplane - Assignment B1", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("../projects/aeroplane/shaders/vertexShader.vs", "../projects/aeroplane/shaders/fragmentShader.fs");
    Shader lightCubeShader("../projects/aeroplane/shaders/lightVertexShader.vs", "../projects/aeroplane/shaders/lightShader.fs");

    // Cube with normals: pos(3) + normal(3) = 6 floats per vertex
    float cubeVerts[] = {
        // Back face
        -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,0.5f,-0.5f, 0,0,-1,
         0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,-0.5f,-0.5f, 0,0,-1,
        // Front face
        -0.5f,-0.5f,0.5f, 0,0,1,  0.5f,-0.5f,0.5f, 0,0,1,  0.5f,0.5f,0.5f, 0,0,1,
         0.5f,0.5f,0.5f, 0,0,1, -0.5f,0.5f,0.5f, 0,0,1, -0.5f,-0.5f,0.5f, 0,0,1,
        // Left face
        -0.5f,0.5f,0.5f, -1,0,0, -0.5f,0.5f,-0.5f, -1,0,0, -0.5f,-0.5f,-0.5f, -1,0,0,
        -0.5f,-0.5f,-0.5f, -1,0,0, -0.5f,-0.5f,0.5f, -1,0,0, -0.5f,0.5f,0.5f, -1,0,0,
        // Right face
        0.5f,0.5f,0.5f, 1,0,0,  0.5f,0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,-0.5f, 1,0,0,
        0.5f,-0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,0.5f, 1,0,0,  0.5f,0.5f,0.5f, 1,0,0,
        // Bottom face
        -0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,0.5f, 0,-1,0,
         0.5f,-0.5f,0.5f, 0,-1,0, -0.5f,-0.5f,0.5f, 0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,
        // Top face
        -0.5f,0.5f,-0.5f, 0,1,0,  0.5f,0.5f,-0.5f, 0,1,0,  0.5f,0.5f,0.5f, 0,1,0,
         0.5f,0.5f,0.5f, 0,1,0, -0.5f,0.5f,0.5f, 0,1,0, -0.5f,0.5f,-0.5f, 0,1,0,
    };
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1,&cubeVAO); glGenBuffers(1,&cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Light cube VAO (reuses same VBO, only position)
    unsigned int lightVAO;
    glGenVertexArrays(1,&lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    // Cylinder with normals
    vector<float> cylVerts;
    int cylSegs = 24;
    for (int i=0; i<cylSegs; i++) {
        float a1 = 2.0f*3.14159f*i/cylSegs, a2 = 2.0f*3.14159f*(i+1)/cylSegs;
        float c1=cos(a1),s1=sin(a1),c2=cos(a2),s2=sin(a2);
        // Side quad (2 triangles), normals point outward
        float verts[] = {
            c1*0.5f,-0.5f,s1*0.5f, c1,0,s1,  c2*0.5f,-0.5f,s2*0.5f, c2,0,s2,  c2*0.5f,0.5f,s2*0.5f, c2,0,s2,
            c1*0.5f,-0.5f,s1*0.5f, c1,0,s1,  c2*0.5f,0.5f,s2*0.5f, c2,0,s2,  c1*0.5f,0.5f,s1*0.5f, c1,0,s1,
        };
        for (float v : verts) cylVerts.push_back(v);
    }
    unsigned int cylVAO, cylVBO;
    glGenVertexArrays(1,&cylVAO); glGenBuffers(1,&cylVBO);
    glBindVertexArray(cylVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylVBO);
    glBufferData(GL_ARRAY_BUFFER, cylVerts.size()*sizeof(float), cylVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Sphere with normals (for nose, tail cap)
    vector<float> sphVerts;
    int sphStacks = 16, sphSlices = 24;
    const float PI = 3.14159265f;
    for (int i = 0; i < sphStacks; i++) {
        float p1 = PI * i / sphStacks, p2 = PI * (i + 1) / sphStacks;
        for (int j = 0; j < sphSlices; j++) {
            float t1 = 2*PI*j/sphSlices, t2 = 2*PI*(j+1)/sphSlices;
            // 4 corners of the quad
            float v[4][6];
            float sp1=sin(p1),cp1=cos(p1),sp2=sin(p2),cp2=cos(p2);
            float ct1=cos(t1),st1=sin(t1),ct2=cos(t2),st2=sin(t2);
            // pos = normal * 0.5 (unit sphere radius 0.5)
            v[0][0]=sp1*ct1*0.5f; v[0][1]=cp1*0.5f; v[0][2]=sp1*st1*0.5f;
            v[0][3]=sp1*ct1;      v[0][4]=cp1;      v[0][5]=sp1*st1;
            v[1][0]=sp1*ct2*0.5f; v[1][1]=cp1*0.5f; v[1][2]=sp1*st2*0.5f;
            v[1][3]=sp1*ct2;      v[1][4]=cp1;      v[1][5]=sp1*st2;
            v[2][0]=sp2*ct2*0.5f; v[2][1]=cp2*0.5f; v[2][2]=sp2*st2*0.5f;
            v[2][3]=sp2*ct2;      v[2][4]=cp2;      v[2][5]=sp2*st2;
            v[3][0]=sp2*ct1*0.5f; v[3][1]=cp2*0.5f; v[3][2]=sp2*st1*0.5f;
            v[3][3]=sp2*ct1;      v[3][4]=cp2;      v[3][5]=sp2*st1;
            // Triangle 1: v0, v1, v2
            for (int k : {0,1,2}) for (int f=0;f<6;f++) sphVerts.push_back(v[k][f]);
            // Triangle 2: v0, v2, v3
            for (int k : {0,2,3}) for (int f=0;f<6;f++) sphVerts.push_back(v[k][f]);
        }
    }
    int sphVertCount = (int)sphVerts.size() / 6;
    unsigned int sphVAO, sphVBO;
    glGenVertexArrays(1,&sphVAO); glGenBuffers(1,&sphVBO);
    glBindVertexArray(sphVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphVBO);
    glBufferData(GL_ARRAY_BUFFER, sphVerts.size()*sizeof(float), sphVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Point light positions
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, 0.3f, -1.0f),  // cabin ceiling light
        glm::vec3(0.0f, 3.0f, 5.0f),   // above tail area
    };

    // Print controls
    cout << "\n========== 3D AIRPLANE SIMULATOR - ASSIGNMENT B1 ==========" << endl;
    cout << "\n=== MOVEMENT ===" << endl;
    cout << "  W/S - Forward/Backward  |  A/D - Left/Right  |  E/R - Up/Down" << endl;
    cout << "  X - Pitch  |  Y - Yaw  |  Z - Roll" << endl;
    cout << "\n=== FEATURES ===" << endl;
    cout << "  G - Toggle fan  |  L - Toggle all lights  |  O - Door  |  P - Windows" << endl;
    cout << "  T - Takeoff  |  U - Landing" << endl;
    cout << "\n=== LIGHTING ===" << endl;
    cout << "  1 - Directional light  |  2 - Point lights  |  3 - Spot light" << endl;
    cout << "  5 - Ambient  |  6 - Diffuse  |  7 - Specular" << endl;
    cout << "\n=== CAMERA ===" << endl;
    cout << "  C - Cycle mode (Follow/Bird/Free/Inside)  |  F - Orbit  |  V - Split view" << endl;
    cout << "  Arrow keys - Free cam movement  |  Scroll - Zoom" << endl;
    cout << "  ESC - Exit" << endl;
    cout << "============================================================\n" << endl;

    // Lambda to render the scene with a given shader, view, projection
    auto renderScene = [&](Shader& shader, glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos) {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", viewPos);

        // Set light toggles
        shader.setBool("dirLightOn", dirLightOn && allLightsOn);
        shader.setBool("pointLightOn", pointLightOn && allLightsOn);
        shader.setBool("spotLightOn", spotLightOn && allLightsOn);
        shader.setBool("ambientOn", ambientOn);
        shader.setBool("diffuseOn", diffuseOn);
        shader.setBool("specularOn", specularOn);

        // Directional light (sun)
        shader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        shader.setVec3("dirLight.ambient", glm::vec3(0.15f));
        shader.setVec3("dirLight.diffuse", glm::vec3(0.6f));
        shader.setVec3("dirLight.specular", glm::vec3(0.4f));

        // Point lights (transformed with plane)
        for (int i=0; i<2; i++) {
            glm::mat4 pt = glm::mat4(1.0f);
            pt = glm::translate(pt, planePos);
            pt = glm::rotate(pt, glm::radians(planeYaw), glm::vec3(0,1,0));
            glm::vec3 wPos = glm::vec3(pt * glm::vec4(pointLightPositions[i], 1.0f));
            string base = "pointLights[" + to_string(i) + "].";
            shader.setVec3(base+"position", wPos);
            shader.setVec3(base+"ambient", glm::vec3(0.1f));
            shader.setVec3(base+"diffuse", glm::vec3(0.8f, 0.8f, 0.7f));
            shader.setVec3(base+"specular", glm::vec3(0.5f));
            shader.setFloat(base+"k_c", 1.0f);
            shader.setFloat(base+"k_l", 0.09f);
            shader.setFloat(base+"k_q", 0.032f);
        }

        // Spot light (nose landing light)
        glm::mat4 spt = glm::mat4(1.0f);
        spt = glm::translate(spt, planePos);
        spt = glm::rotate(spt, glm::radians(planeYaw), glm::vec3(0,1,0));
        spt = glm::rotate(spt, glm::radians(planePitch), glm::vec3(1,0,0));
        glm::vec3 spotPos = glm::vec3(spt * glm::vec4(0, 0.3f, -3.5f, 1.0f));
        glm::vec3 spotDir = glm::vec3(spt * glm::vec4(0, 0.3f, -1.0f, 0.0f));
        shader.setVec3("spotLight.position", spotPos);
        shader.setVec3("spotLight.direction", spotDir);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(50.0f)));
        shader.setVec3("spotLight.ambient", glm::vec3(0.1f));
        shader.setVec3("spotLight.diffuse", glm::vec3(2.0f, 2.0f, 1.0f));
        shader.setVec3("spotLight.specular", glm::vec3(0.8f));
        shader.setFloat("spotLight.k_c", 2.0f);
        shader.setFloat("spotLight.k_l", 0.045f);
        shader.setFloat("spotLight.k_q", 0.0075f);

        // Plane transform
        glm::mat4 P = glm::mat4(1.0f);
        P = glm::translate(P, planePos);
        P = glm::rotate(P, glm::radians(planeYaw), glm::vec3(0,1,0));
        P = glm::rotate(P, glm::radians(planePitch), glm::vec3(1,0,0));
        P = glm::rotate(P, glm::radians(planeRoll), glm::vec3(0,0,1));

        // --- FUSELAGE (cylindrical body, axis along Z) ---
        glm::mat4 m = glm::translate(P, glm::vec3(0, 0, 0));
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(1,0,0));
        m = glm::scale(m, glm::vec3(1.8f, 7.0f, 1.4f));
        drawCylinder(shader, cylVAO, cylSegs, m, glm::vec3(0.15f), glm::vec3(0.92f), glm::vec3(0.3f), 32.0f);

        // Nose (sphere, front of fuselage)
        m = glm::translate(P, glm::vec3(0, 0, -3.5f));
        m = glm::scale(m, glm::vec3(1.8f, 1.4f, 2.0f));
        drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.15f), glm::vec3(0.9f), glm::vec3(0.4f), 64.0f);

        // Cockpit windshield (on top of nose sphere)
        m = glm::translate(P, glm::vec3(0, 0.55f, -3.6f));
        m = glm::scale(m, glm::vec3(1.2f, 0.5f, 1.6f));
        drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.05f), glm::vec3(0.2f, 0.4f, 0.75f), glm::vec3(0.9f), 128.0f);

        // Tail taper (sphere, rear)
        m = glm::translate(P, glm::vec3(0, 0, 3.5f));
        m = glm::scale(m, glm::vec3(1.8f, 1.4f, 2.5f));
        drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.15f), glm::vec3(0.9f), glm::vec3(0.3f), 32.0f);

        // Tail cone tip
        m = glm::translate(P, glm::vec3(0, 0.2f, 4.8f));
        m = glm::scale(m, glm::vec3(0.6f, 0.5f, 1.5f));
        drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.15f), glm::vec3(0.88f), glm::vec3(0.3f), 32.0f);

        // --- WINGS (blue) ---
        m = glm::scale(glm::translate(P, glm::vec3(-4.0f, 0, 0)), glm::vec3(6.0f, 0.15f, 2.8f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.2f, 0.4f, 0.85f), glm::vec3(0.4f), 32.0f);
        m = glm::scale(glm::translate(P, glm::vec3(4.0f, 0, 0)), glm::vec3(6.0f, 0.15f, 2.8f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.2f, 0.4f, 0.85f), glm::vec3(0.4f), 32.0f);

        // --- ENGINES (cylindrical nacelles under wings) ---
        glm::mat4 eng;
        for (float ex : {-3.0f, 3.0f}) {
            // Engine nacelle (cylinder along Z)
            eng = glm::translate(P, glm::vec3(ex, -0.6f, 0));
            eng = glm::rotate(eng, glm::radians(90.0f), glm::vec3(1,0,0));
            eng = glm::scale(eng, glm::vec3(0.8f, 1.8f, 0.8f));
            drawCylinder(shader, cylVAO, cylSegs, eng, glm::vec3(0.08f), glm::vec3(0.35f), glm::vec3(0.5f), 64.0f);
            // Engine intake (sphere at front)
            m = glm::translate(P, glm::vec3(ex, -0.6f, -0.9f));
            m = glm::scale(m, glm::vec3(0.82f, 0.82f, 0.5f));
            drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.05f), glm::vec3(0.25f), glm::vec3(0.6f), 64.0f);
        }

        // --- PROPELLERS / FANS ---
        auto drawFan = [&](float px) {
            for (int b=0; b<4; b++) {
                m = glm::translate(P, glm::vec3(px, -0.6f, -0.95f));
                m = glm::rotate(m, glm::radians(propellerRotation + b*90.0f), glm::vec3(0,0,1));
                m = glm::scale(m, glm::vec3(2.0f, 0.1f, 0.15f));
                drawCube(shader, cubeVAO, m, glm::vec3(0.02f), glm::vec3(0.15f), glm::vec3(0.3f), 32.0f);
            }
        };
        drawFan(-3.0f); drawFan(3.0f);

        // --- TAIL ---
        m = glm::scale(glm::translate(P, glm::vec3(0, 1.8f, 4.0f)), glm::vec3(0.15f, 2.2f, 1.2f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.2f, 0.4f, 0.85f), glm::vec3(0.3f), 32.0f);
        m = glm::scale(glm::translate(P, glm::vec3(0, 2.6f, 4.0f)), glm::vec3(3.0f, 0.15f, 1.0f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.2f, 0.4f, 0.85f), glm::vec3(0.3f), 32.0f);

        // --- DOOR ---
        glm::mat4 dm = glm::translate(P, glm::vec3(-0.91f - doorAngle*0.5f, -0.2f, 1.5f));
        dm = glm::rotate(dm, glm::radians(-doorAngle*60.0f), glm::vec3(0,1,0));
        dm = glm::scale(dm, glm::vec3(0.05f, 1.2f, 0.9f));
        drawCube(shader, cubeVAO, dm, glm::vec3(0.06f), glm::vec3(0.5f, 0.3f, 0.15f), glm::vec3(0.2f), 16.0f);

        // --- WINDOWS (small spheres embedded in fuselage) ---
        float wAlpha = windowOpen ? 0.4f : 0.7f;
        for (int side = -1; side <= 1; side += 2) {
            for (int i=0; i<10; i++) {
                m = glm::translate(P, glm::vec3(side*0.88f, 0.2f, -2.8f + i*0.6f));
                m = glm::scale(m, glm::vec3(0.08f, 0.25f, 0.3f));
                drawSphere(shader, sphVAO, sphVertCount, m, glm::vec3(0.05f), glm::vec3(0.5f*wAlpha, 0.8f*wAlpha, 1.0f*wAlpha), glm::vec3(0.8f), 128.0f);
            }
        }

        // --- LANDING GEAR ---
        auto drawGear = [&](glm::vec3 pos) {
            m = glm::scale(glm::translate(P, pos), glm::vec3(0.15f, 0.7f, 0.15f));
            drawCube(shader, cubeVAO, m, glm::vec3(0.02f), glm::vec3(0.1f), glm::vec3(0.2f), 16.0f);
            // Wheel
            glm::mat4 wm = glm::translate(P, pos + glm::vec3(0, -0.4f, 0));
            wm = glm::rotate(wm, glm::radians(90.0f), glm::vec3(0,0,1));
            wm = glm::scale(wm, glm::vec3(0.25f, 0.3f, 0.25f));
            drawCylinder(shader, cylVAO, cylSegs, wm, glm::vec3(0.02f), glm::vec3(0.08f), glm::vec3(0.1f), 16.0f);
        };
        drawGear(glm::vec3(0, -0.9f, -2.5f));
        drawGear(glm::vec3(-1.5f, -0.9f, 0.5f));
        drawGear(glm::vec3(1.5f, -0.9f, 0.5f));

        // ===== CABIN INTERIOR =====
        // Floor (inside cylindrical fuselage)
        m = glm::scale(glm::translate(P, glm::vec3(0, -0.55f, 0)), glm::vec3(1.5f, 0.05f, 6.5f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.25f, 0.22f, 0.2f), glm::vec3(0.1f), 8.0f);

        // Ceiling
        m = glm::scale(glm::translate(P, glm::vec3(0, 0.55f, 0)), glm::vec3(1.5f, 0.05f, 6.5f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.1f), glm::vec3(0.85f), glm::vec3(0.2f), 16.0f);

        // Aisle (darker strip on floor)
        m = glm::scale(glm::translate(P, glm::vec3(0, -0.62f, 0)), glm::vec3(0.4f, 0.02f, 6.0f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.03f), glm::vec3(0.15f, 0.15f, 0.18f), glm::vec3(0.1f), 8.0f);

        // Seats (rows of 3+3 with aisle)
        for (int row = 0; row < 8; row++) {
            float zPos = -2.5f + row * 0.7f;
            for (int side = -1; side <= 1; side += 2) {
                for (int col = 0; col < 2; col++) {
                    float xPos = side * (0.35f + col * 0.35f);
                    // Seat bottom (blue cushion)
                    m = glm::scale(glm::translate(P, glm::vec3(xPos, -0.45f, zPos)), glm::vec3(0.28f, 0.12f, 0.3f));
                    drawCube(shader, cubeVAO, m, glm::vec3(0.02f), glm::vec3(0.1f, 0.15f, 0.5f), glm::vec3(0.2f), 16.0f);
                    // Seat back
                    m = glm::scale(glm::translate(P, glm::vec3(xPos, -0.2f, zPos + 0.13f)), glm::vec3(0.28f, 0.4f, 0.06f));
                    drawCube(shader, cubeVAO, m, glm::vec3(0.02f), glm::vec3(0.1f, 0.15f, 0.5f), glm::vec3(0.2f), 16.0f);
                }
            }
        }

        // Cockpit dashboard
        m = glm::scale(glm::translate(P, glm::vec3(0, 0.0f, -3.2f)), glm::vec3(1.4f, 0.8f, 0.15f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.03f), glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(0.3f), 32.0f);

        // Cockpit yoke (two small cylinders)
        for (int s=-1; s<=1; s+=2) {
            glm::mat4 yk = glm::translate(P, glm::vec3(s*0.35f, -0.1f, -3.05f));
            yk = glm::rotate(yk, glm::radians(90.0f), glm::vec3(1,0,0));
            yk = glm::scale(yk, glm::vec3(0.06f, 0.3f, 0.06f));
            drawCylinder(shader, cylVAO, cylSegs, yk, glm::vec3(0.02f), glm::vec3(0.08f), glm::vec3(0.2f), 16.0f);
        }

        // ===== GROUND / RUNWAY =====
        m = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -2.0f, 0)), glm::vec3(60.0f, 0.1f, 120.0f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.05f), glm::vec3(0.25f, 0.28f, 0.22f), glm::vec3(0.05f), 4.0f);
        // Runway strip
        m = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.94f, 0)), glm::vec3(4.0f, 0.02f, 100.0f));
        drawCube(shader, cubeVAO, m, glm::vec3(0.08f), glm::vec3(0.35f), glm::vec3(0.1f), 8.0f);
        // Center line markings
        for (int i=-12; i<12; i++) {
            m = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.93f, i*4.5f)), glm::vec3(0.2f, 0.02f, 2.0f));
            drawCube(shader, cubeVAO, m, glm::vec3(0.2f), glm::vec3(0.95f), glm::vec3(0.3f), 8.0f);
        }

        // Draw light source markers
        lightCubeShader.use();
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        for (int i=0; i<2; i++) {
            if (!pointLightOn || !allLightsOn) continue;
            glm::mat4 pt2 = glm::translate(glm::mat4(1.0f), planePos);
            pt2 = glm::rotate(pt2, glm::radians(planeYaw), glm::vec3(0,1,0));
            glm::vec3 wp = glm::vec3(pt2 * glm::vec4(pointLightPositions[i], 1.0f));
            m = glm::scale(glm::translate(glm::mat4(1.0f), wp), glm::vec3(0.15f));
            lightCubeShader.setMat4("model", m);
            lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.8f));
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    };

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (engineRunning) {
            propellerRotation += 1000.0f * deltaTime;
            if (propellerRotation > 360.0f) propellerRotation -= 360.0f;
        }
        if (takingOff) {
            takeoffProgress += deltaTime * 0.3f;
            if (takeoffProgress >= 1.0f) { takeoffProgress = 1.0f; takingOff = false; }
            planePos.z -= 15.0f * deltaTime;
            planePos.y = takeoffProgress * 8.0f;
            planePitch = -takeoffProgress * 15.0f;
        }
        if (landing) {
            landingProgress += deltaTime * 0.3f;
            if (landingProgress >= 1.0f) { landingProgress = 1.0f; landing = false; planePos.y = 0; }
            planePos.z += 15.0f * deltaTime;
            planePos.y = (1.0f - landingProgress) * 8.0f;
            planePitch = (1.0f - landingProgress) * 15.0f;
        }

        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto getView = [&](CamMode mode) -> glm::mat4 {
            if (mode == BIRD_EYE)
                return myLookAt(planePos + glm::vec3(0, 20, 0.1f), planePos, glm::vec3(0, 0, -1));
            if (mode == FOLLOW_PLANE) {
                glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(planeYaw), glm::vec3(0,1,0));
                glm::vec3 off = glm::vec3(r * glm::vec4(0, 3, 10, 1));
                return myLookAt(planePos + off, planePos, glm::vec3(0,1,0));
            }
            if (mode == INSIDE_CABIN) {
                glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(planeYaw), glm::vec3(0,1,0));
                glm::vec3 eyeOff = glm::vec3(r * glm::vec4(0, 0.1f, 1.5f, 1));
                glm::vec3 lookOff = glm::vec3(r * glm::vec4(0, 0.1f, -3.0f, 1));
                return myLookAt(planePos + eyeOff, planePos + lookOff, glm::vec3(0,1,0));
            }
            return myLookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        };

        if (splitView) {
            int w = SCR_WIDTH, h = SCR_HEIGHT;
            glm::mat4 proj = glm::perspective(glm::radians(cameraFov), (float)(w/2)/(float)(h/2), 0.1f, 200.0f);
            struct VP { int x,y,w,h; CamMode mode; } vps[] = {
                {0, h/2, w/2, h/2, FOLLOW_PLANE},
                {w/2, h/2, w/2, h/2, BIRD_EYE},
                {0, 0, w/2, h/2, FREE_CAM},
                {w/2, 0, w/2, h/2, INSIDE_CABIN},
            };
            for (auto& vp : vps) {
                glViewport(vp.x, vp.y, vp.w, vp.h);
                glScissor(vp.x, vp.y, vp.w, vp.h);
                glEnable(GL_SCISSOR_TEST);
                glClear(GL_DEPTH_BUFFER_BIT);
                glm::mat4 v = getView(vp.mode);
                glm::vec3 vPos = (vp.mode == FREE_CAM) ? cameraPos : planePos + glm::vec3(0, 3, 10);
                renderScene(lightingShader, v, proj, vPos);
                glDisable(GL_SCISSOR_TEST);
            }
            glViewport(0, 0, w, h);
        } else {
            glm::mat4 proj = glm::perspective(glm::radians(cameraFov), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 200.0f);
            glm::mat4 v = getView(cameraMode);
            glm::vec3 vPos = (cameraMode == FREE_CAM) ? cameraPos : planePos + glm::vec3(0, 3, 10);
            renderScene(lightingShader, v, proj, vPos);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&cubeVAO); glDeleteBuffers(1,&cubeVBO);
    glDeleteVertexArrays(1,&cylVAO); glDeleteBuffers(1,&cylVBO);
    glDeleteVertexArrays(1,&sphVAO); glDeleteBuffers(1,&sphVBO);
    glDeleteVertexArrays(1,&lightVAO);
    glfwTerminate();
    return 0;
}

void drawCube(Shader& s, unsigned int VAO, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin) {
    s.use();
    s.setMat4("model", model);
    s.setVec3("material.ambient", amb);
    s.setVec3("material.diffuse", diff);
    s.setVec3("material.specular", spec);
    s.setFloat("material.shininess", shin);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCylinder(Shader& s, unsigned int VAO, int segs, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin) {
    s.use();
    s.setMat4("model", model);
    s.setVec3("material.ambient", amb);
    s.setVec3("material.diffuse", diff);
    s.setVec3("material.specular", spec);
    s.setFloat("material.shininess", shin);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, segs * 6);
}

void drawSphere(Shader& s, unsigned int VAO, int vertCount, glm::mat4 model, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin) {
    s.use();
    s.setMat4("model", model);
    s.setVec3("material.ambient", amb);
    s.setVec3("material.diffuse", diff);
    s.setVec3("material.specular", spec);
    s.setFloat("material.shininess", shin);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
}

void processInput(GLFWwindow* w) {
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(w, true);

    float speed = 5.0f * deltaTime, rotSpeed = 50.0f * deltaTime;

    // Movement
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 fwd(sin(glm::radians(planeYaw)), 0, -cos(glm::radians(planeYaw)));
        planePos += fwd * speed;
    }
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 fwd(sin(glm::radians(planeYaw)), 0, -cos(glm::radians(planeYaw)));
        planePos -= fwd * speed;
    }
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 right(cos(glm::radians(planeYaw)), 0, sin(glm::radians(planeYaw)));
        planePos -= right * speed;
    }
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right(cos(glm::radians(planeYaw)), 0, sin(glm::radians(planeYaw)));
        planePos += right * speed;
    }
    if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) planePos.y += speed;
    if (glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS) planePos.y -= speed;

    // Rotations
    if (glfwGetKey(w, GLFW_KEY_X) == GLFW_PRESS) { planePitch += rotSpeed; if (planePitch > 360) planePitch -= 360; }
    if (glfwGetKey(w, GLFW_KEY_Y) == GLFW_PRESS) { planeYaw += rotSpeed; if (planeYaw > 360) planeYaw -= 360; }
    if (glfwGetKey(w, GLFW_KEY_Z) == GLFW_PRESS) { planeRoll += rotSpeed; if (planeRoll > 360) planeRoll -= 360; }

    // Toggle macros
    #define TOGGLE_KEY(key, var, msg1, msg2) { \
        static bool _p=false; \
        if (glfwGetKey(w,key)==GLFW_PRESS && !_p) { var=!var; _p=true; cout<<(var?msg1:msg2)<<endl; } \
        if (glfwGetKey(w,key)==GLFW_RELEASE) _p=false; }

    TOGGLE_KEY(GLFW_KEY_G, engineRunning, "Engine started", "Engine stopped");
    TOGGLE_KEY(GLFW_KEY_L, allLightsOn, "All lights ON", "All lights OFF");
    TOGGLE_KEY(GLFW_KEY_O, doorOpen, "Door opening", "Door closing");
    TOGGLE_KEY(GLFW_KEY_P, windowOpen, "Window shades opened", "Window shades closed");

    // Individual light toggles
    TOGGLE_KEY(GLFW_KEY_1, dirLightOn, "Directional light ON", "Directional light OFF");
    TOGGLE_KEY(GLFW_KEY_2, pointLightOn, "Point lights ON", "Point lights OFF");
    TOGGLE_KEY(GLFW_KEY_3, spotLightOn, "Spot light ON", "Spot light OFF");
    TOGGLE_KEY(GLFW_KEY_5, ambientOn, "Ambient ON", "Ambient OFF");
    TOGGLE_KEY(GLFW_KEY_6, diffuseOn, "Diffuse ON", "Diffuse OFF");
    TOGGLE_KEY(GLFW_KEY_7, specularOn, "Specular ON", "Specular OFF");
    #undef TOGGLE_KEY

    // Door animation
    if (doorOpen && doorAngle < 1.0f) { doorAngle += deltaTime; if (doorAngle > 1) doorAngle = 1; }
    else if (!doorOpen && doorAngle > 0.0f) { doorAngle -= deltaTime; if (doorAngle < 0) doorAngle = 0; }

    // Takeoff
    static bool tP=false;
    if (glfwGetKey(w,GLFW_KEY_T)==GLFW_PRESS && !tP) { takingOff=true; landing=false; takeoffProgress=0; engineRunning=true; tP=true; cout<<"Takeoff!"<<endl; }
    if (glfwGetKey(w,GLFW_KEY_T)==GLFW_RELEASE) tP=false;

    // Landing
    static bool uP=false;
    if (glfwGetKey(w,GLFW_KEY_U)==GLFW_PRESS && !uP) { landing=true; takingOff=false; landingProgress=0; uP=true; cout<<"Landing!"<<endl; }
    if (glfwGetKey(w,GLFW_KEY_U)==GLFW_RELEASE) uP=false;

    // Orbit
    if (glfwGetKey(w, GLFW_KEY_F) == GLFW_PRESS) {
        orbitAngle += rotSpeed * 2;
        cameraPos.x = planePos.x + orbitRadius * cos(glm::radians(orbitAngle));
        cameraPos.z = planePos.z + orbitRadius * sin(glm::radians(orbitAngle));
        cameraPos.y = planePos.y + 5;
        cameraFront = glm::normalize(planePos - cameraPos);
        cameraMode = FREE_CAM;
    }

    // Camera mode
    static bool cP=false;
    if (glfwGetKey(w,GLFW_KEY_C)==GLFW_PRESS && !cP) {
        cameraMode = (CamMode)((cameraMode + 1) % 4);
        cP=true;
        const char* names[] = {"Free Camera","Bird's Eye","Follow Plane","Inside Cabin"};
        cout << "Camera: " << names[cameraMode] << endl;
    }
    if (glfwGetKey(w,GLFW_KEY_C)==GLFW_RELEASE) cP=false;

    // Split view toggle
    static bool vP=false;
    if (glfwGetKey(w,GLFW_KEY_4)==GLFW_PRESS && !vP) { splitView=!splitView; vP=true; cout<<(splitView?"Split view ON":"Split view OFF")<<endl; }
    if (glfwGetKey(w,GLFW_KEY_4)==GLFW_RELEASE) vP=false;

    // Free cam
    if (cameraMode == FREE_CAM) {
        float cs = 10.0f * deltaTime;
        if (glfwGetKey(w,GLFW_KEY_UP)==GLFW_PRESS) cameraPos += cs * cameraFront;
        if (glfwGetKey(w,GLFW_KEY_DOWN)==GLFW_PRESS) cameraPos -= cs * cameraFront;
        if (glfwGetKey(w,GLFW_KEY_LEFT)==GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cs;
        if (glfwGetKey(w,GLFW_KEY_RIGHT)==GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cs;
    }
}

void mouse_callback(GLFWwindow* w, double xpos, double ypos) {
    if (cameraMode != FREE_CAM) return;
    if (firstMouse) { lastX=xpos; lastY=ypos; firstMouse=false; }
    float xo=xpos-lastX, yo=lastY-ypos;
    lastX=xpos; lastY=ypos;
    cameraYaw += xo*0.1f; cameraPitch += yo*0.1f;
    if (cameraPitch>89) cameraPitch=89; if (cameraPitch<-89) cameraPitch=-89;
    glm::vec3 f;
    f.x = cos(glm::radians(cameraYaw))*cos(glm::radians(cameraPitch));
    f.y = sin(glm::radians(cameraPitch));
    f.z = sin(glm::radians(cameraYaw))*cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(f);
}

void scroll_callback(GLFWwindow* w, double xo, double yo) {
    cameraFov -= (float)yo;
    if (cameraFov<1) cameraFov=1; if (cameraFov>90) cameraFov=90;
}

void framebuffer_size_callback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
}
