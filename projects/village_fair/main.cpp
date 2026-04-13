#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "directionalLight.h"
#include "pointLight.h"
#include "spotLight.h"
#include "sphere.h"
#include "sphereWithTexture.h"
#include "BanyanTree.h"
#include "MiniBanyanForest.h"
#include "CircusTent.h"
#include "lamppost.h"
#include "collision.h"
#include "stb_image.h"

#include <iostream>
#include <stdlib.h>
#include <windows.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void Floor(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void drawCube(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest = 0, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float spec = 1.0f, float shininess = 32.0f);
void drawCube1(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest = 0, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float spec = 1.0f, float shininess = 32.0f);
void Road(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Boundary(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Gate(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Shop(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color, unsigned int texture, unsigned int itemTex1 = 0, unsigned int itemTex2 = 0);
void ShopVariant(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 roofTint, unsigned int texture, unsigned int itemTex1 = 0, unsigned int itemTex2 = 0);
void BalloonBunch(Shader ourShader, glm::mat4 moveMatrix);
void FlagBunting(Shader ourShader, glm::mat4 moveMatrix, float length);
void VendorCart(Shader ourShader, glm::mat4 moveMatrix);
void Well(Shader ourShader, glm::mat4 moveMatrix);
void drawFenceBoundary(Shader& shader, glm::mat4 moveMatrix, unsigned int woodTex, unsigned int plainTex, unsigned int signTex, float time);
void load_texture(unsigned int& texture, string image_name, GLenum format);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void SetupPointLight(PointLight &pointLight, Shader ourShader, int lightNum);
void Stage(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void FoodPlace(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void read_file(string file_name, vector<float> &vec);
long long nCr(int n, int r);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices);
void drawCurve(Shader ourShader, glm::mat4 moveMatrix, unsigned int VAO, vector<int> indices, glm::vec4 color = glm::vec4(1.0f), float rotateAngleTest_Y = 0);
void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix);
void drawPlayerThirdPerson(Shader& s, glm::mat4 rootMatrix, unsigned int plainTex, float walkPhase);
void drawPlayerFirstPerson(Shader& s, glm::mat4 viewmodelMatrix, unsigned int plainTex, float walkPhase);
void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z = 0, glm::mat4 parentMove = glm::mat4(1.0f));
void PirateShip(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z = 0);
void drawKite(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 barColor, float scale, glm::vec3 position);
void drawJar(Shader& shader, glm::mat4 moveMatrix, glm::vec4 color, float radius, float height, int segments);
void drawKiteWithStick(Shader& shader, glm::mat4 moveMatrix, glm::vec4 kiteColor, glm::vec4 stickColor, float scale, glm::vec3 position);
void drawBezierRiver(Shader& shader, glm::mat4 moveMatrix, unsigned int VAO, std::vector<int>& indices, glm::vec4 color);
void seat(Shader ourShader, glm::mat4 moveMatrix);
void drawRealisticTree(Shader& shader, glm::mat4 modelMatrix, unsigned int trunkVAO, unsigned int canopyVAO, std::vector<int>& trunkIndices, std::vector<int>& canopyIndices);
void garden(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Carousal(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z = 0);

void glass(Shader& ourShader, unsigned int& VAO, float tx, float ty, float tz, float sx, float sy, float sz);
void drawShopRow1(Shader ourShader, glm::mat4 moveMatrix, bool useTexture);
void drawShopRow2(Shader ourShader, glm::mat4 moveMatrix, bool useTexture);
void drawShopRow3(Shader ourShader, glm::mat4 moveMatrix, bool useTexture);
void drawUmbrella(Shader ourShader, glm::mat4 moveMatrix);
// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 0.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

float rotateAngleTest_Y = 0.0;


// camera — entry view from bottom-left, facing toward fair center
Camera camera(glm::vec3(-22.0f, 5.0f, -20.0f), glm::vec3(0,1,0), 42.0f, -12.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 2.5;
float lookAtX = 0.0, lookAtY = 1.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
float lastTime = 0.0f, lastTimeSky = 0.0f;


// Light initialization
const int noOfPointLights = 13;  // 4 tent corners + 9 lamp posts

glm::vec3 lightPositions[] = {
        glm::vec3(-0.95f, 1.4f, -2.7f),         //Directional Light
        glm::vec3(10.2f, 1.35f, 14.48f),
        glm::vec3(6.2f, 1.35f, -4.52f),
        glm::vec3(10.2f, 1.35f, 1.48f),
        glm::vec3(6.2f, 1.35f, 7.98f),
        glm::vec3(5.0f, 1.35f, 8.18f)

};

glm::vec3 lightDirections[] = {
    glm::vec3(-2.0f, -0.2f, -1.3f)
};


DirectionalLight directionalLight(-lightPositions[0], glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(0.2f, 0.2f, 0.2f, 0.2f), 1);
SpotLight spotLight(lightPositions[5], lightDirections[0], 4.5f, 7.5f, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.0014f, 0.000007f, 1);
PointLight pointLight1(lightPositions[1], glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 1);
PointLight pointLight2(lightPositions[2], glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 2);
PointLight pointLight3(lightPositions[3], glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 3);
PointLight pointLight4(lightPositions[4], glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 4);

// Lamp post point lights (5-13) — bulb at local offset (0.23, 1.66, 0.0) from base
// Warm white, moderate range for road lighting
glm::vec4 lpAmb(0.03f, 0.03f, 0.02f, 1.0f);
glm::vec4 lpDiff(2.5f, 2.2f, 1.5f, 1.0f);
glm::vec4 lpSpec(0.6f, 0.6f, 0.5f, 1.0f);
// Left road (X=-12): bases at z=5, -1, -7 → bulb offset (+0.23, +1.66, 0)
PointLight pointLight5( glm::vec3(-11.77f, 1.24f,  5.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 5);
PointLight pointLight6( glm::vec3(-11.77f, 1.24f, -1.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 6);
PointLight pointLight7( glm::vec3(-11.77f, 1.24f, -7.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 7);
// Right road (X=10): bases at z=5, -1, -7
PointLight pointLight8( glm::vec3(10.23f, 1.24f,  5.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 8);
PointLight pointLight9( glm::vec3(10.23f, 1.24f, -1.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 9);
PointLight pointLight10(glm::vec3(10.23f, 1.24f, -7.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 10);
// Bottom road (Z=-11): bases at x=-6, 0, 6
PointLight pointLight11(glm::vec3(-5.77f, 1.24f, -11.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 11);
PointLight pointLight12(glm::vec3( 0.23f, 1.24f, -11.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 12);
PointLight pointLight13(glm::vec3( 6.23f, 1.24f, -11.0f), lpAmb, lpDiff, lpSpec, 1.0f, 0.70f, 0.50f, 13);


//***********************************Curve*******************
vector <float> cntrlPoints, cntrlPointsBoat, cntrlPointsRotor, cntrlPointsCylinder, cntrlPointsCarousal, cntrlPointsHead;
vector <float> coordinates, coordinatesBoat, coordinatesRotor, coordinatesCylinder, coordinatesCarousal, coordinatesHead;
vector <float> normals, normalsBoat, normalsRotor, normalsCylinder, normalsCarousal, normalsHead;
vector <int> indices, indicesBoat, indicesRotor, indicesCylinder, indicesCarousal, indicesHead;
vector <float> vertices, verticesBoat, verticesRotor, verticesCylinder, verticesCarousal, verticesHead;

const double pi = 3.14159265389;
const int nt = 40;
const int ntheta = 30;

unsigned int cubeVAO, cubeVBO, cubeEBO;
unsigned int bezierVAO, boatVAO, rotorVAO, cylinderVAO, carousalVAO, headVAO;

// Sphere VAO variables
unsigned int sphereVAO, sphereVBO, sphereEBO;
std::vector<float> sphereVertices;
std::vector<unsigned int> sphereIndices;
int sphereIndexCount = 0;

void generateSphere(float radius, int sectors, int stacks) {
    sphereVertices.clear();
    sphereIndices.clear();

    float sectorStep = 2 * M_PI / sectors;
    float stackStep  = M_PI / stacks;

    for (int i = 0; i <= stacks; i++) {
        float stackAngle = M_PI / 2 - i * stackStep;
        float xy = radius * cos(stackAngle);
        float z  = radius * sin(stackAngle);
        for (int j = 0; j <= sectors; j++) {
            float sectorAngle = j * sectorStep;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            // pos
            sphereVertices.push_back(x);
            sphereVertices.push_back(z); // Y up
            sphereVertices.push_back(y);
            // normal
            sphereVertices.push_back(x / radius);
            sphereVertices.push_back(z / radius);
            sphereVertices.push_back(y / radius);
            // uv
            sphereVertices.push_back((float)j / sectors);
            sphereVertices.push_back((float)i / stacks);
        }
    }
    for (int i = 0; i < stacks; i++) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;
        for (int j = 0; j < sectors; j++, k1++, k2++) {
            if (i != 0) {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }
            if (i != stacks - 1) {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
        }
    }
    sphereIndexCount = sphereIndices.size();

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);
    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

// Add this in the initialization section of main.cpp, after other objects are set up
unsigned int riverVAO;
std::vector<float> riverCoordinates, riverNormals, riverVertices;
std::vector<int> riverIndices;


unsigned int trunkVAO;
std::vector<float> trunkCoordinates, trunkNormals, trunkVertices;
std::vector<int> trunkIndices;

unsigned int canopyVAO;
std::vector<float> canopyCoordinates, canopyNormals, canopyVertices;
std::vector<int> canopyIndices;


// texture
float extra = 4.0f;
float TXmin = 0.0f;
float TXmax = 1.0f + extra;
float TYmin = 0.0f;
float TYmax = 1.0f + extra;



//Switches
float gateAngle = 0.0f;
float isGateOpening = -1.0f;
float gateSpeed = 1.0f;

float speed = 9.0f;

float ferrisWheelAngle = 0.0f;
float ferrisWheelSpeed = 0.01f;
float ferrisIncrement = 0.001f;
bool isFerrisWheelOn = true;

float boatAngle = 0.0f;
float boatSpeed = 0.01f;
bool isBoatOn = true;
float boatMaxAngle = 1.0f;


bool isCarousalOn = true;
float carousalSpeed = 0.005f;
float carousalAngle = 0.0f;
float carousalIncrement = 0.0005f;



//light switches
bool lightingOn = true;
float ambientOn = 1.0;
float diffuseOn = 1.0;
float specularOn = 1.0;
bool emissiveOn = true;
bool dark = false;

float directionalLightOn = 1.0;

// ---- Circus Tent globals (needed by callbacks.h) ----
CircusTent circusTent;
bool  g_insideTent  = false;
glm::vec3 g_tentCenter = glm::vec3(-5.0f, -0.42f, -3.0f);
float g_tentRadius  = 3.5f;
float g_tentFloorY  = -0.42f + 0.1f;
float g_tentCeilY   = -0.42f + 4.5f;
glm::vec3 g_savedCamPos = glm::vec3(0.0f);
float g_savedYaw   = 0.0f;
float g_savedPitch = 0.0f;

// ---- Cursor & FOV globals ----
bool  g_cursorLocked = true;   // starts locked for FPS-style look
float g_fov          = 75.0f;  // field of view (scroll to zoom)

// ---- Player character globals ----
enum CameraMode { CAM_FIRST_PERSON, CAM_CHASE };
CameraMode g_camMode        = CAM_CHASE;
glm::vec3  g_playerPos      = glm::vec3(-18.0f, -0.42f, -16.0f);  // feet on ground, near gate
float      g_playerYaw      = 42.0f;   // degrees, matches initial camera yaw
float      g_playerPitch    = 0.0f;    // vertical look angle
float      g_walkTimer      = 0.0f;    // accumulated walk phase for bobbing
bool       g_playerMoving   = false;   // true when WASD held this frame
// Chase camera smoothing
glm::vec3  g_chaseCamPos    = glm::vec3(-18.0f, 2.0f, -16.0f);
float      g_chaseSmoothSpeed = 5.0f;

// Pond module forward declarations
void initPondShip();
void drawPondShipScene(Shader& shader, glm::mat4 moveMatrix, float time);
float pointLightOn[noOfPointLights] = { 0.0, 0.0, 0.0, 0.0 };
float spotLightOn = 0.0;

// Sun state (updated each frame from time of day)
glm::vec3 g_sunDir   = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_sunColor = glm::vec3(1.0f, 1.0f, 1.0f);
// Manual sun step 0..9. 0..6 = day (7 steps), 7..9 = night (3 steps). Advanced by Q.
int g_sunStep = 1; // start a little after sunrise


// Add this near the top of main.cpp, with other global variables
std::vector<float> riverControlPoints = {
    0.0f, 0.0f, 3.0f,   // Point 1
    2.0f, 0.0f, 2.0f,   // Point 2
    3.0f, 0.0f, 0.0f,   // Point 3
    2.0f, 0.0f, -2.0f,  // Point 4
    0.0f, 0.0f, -3.0f,  // Point 5
    -2.0f, 0.0f, -2.0f, // Point 6
    -3.0f, 0.0f, 0.0f,  // Point 7
    -2.0f, 0.0f, 2.0f,  // Point 8
    0.0f, 0.0f, 3.0f    // Point 9 (same as Point 1 to close the loop)
};
std::vector<float> generateTrunkControlPoints() {
    return {
        0.0f, 0.0f, 0.0f, // Base
        0.1f, 1.0f, 0.0f, // Slight curve
        0.0f, 3.0f, 0.0f  // Top
    };
}

std::vector<float> generateCanopyControlPoints() {
    return {
        -1.5f, 4.0f, 0.0f,  // Left base
        -1.5f, 4.5f, 0.0f,  // Slight upward
        0.0f, 5.5f, 0.0f,   // Peak
        1.5f, 4.5f, 0.0f,   // Slight downward
        1.5f, 4.0f, 0.0f    // Right base
    };
}




// Textures
unsigned int texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9,texture10,garden2,garden1,whiteflower,shoptex,bambooTex;
unsigned int barkTex, leafTex, brickTex2, signTex;
// Shop item textures (billboard sprites)
unsigned int itemChipsTex, itemPopcornTex, itemSlushyTex, itemTeddyTex, itemTeddyMultiTex;
unsigned int itemCottonPinkTex, itemCottonBlueTex;

bool textureOn = false;
int  g_textureMode = 3;     // 0=Off, 1=Pure, 2=Vertex, 3=Fragment (default)
int  g_wrapMode    = 0;     // 0=Repeat, 1=MirroredRepeat, 2=ClampToEdge, 3=ClampToBorder
int  g_filterMode  = 1;     // 0=Nearest, 1=Linear, 2=MipmapNearest, 3=MipmapLinear

const char* g_texModeNames[]   = { "Off", "Pure", "Vertex", "Fragment" };
const char* g_wrapModeNames[]  = { "Repeat", "MirroredRepeat", "ClampToEdge", "ClampToBorder" };
const char* g_filterModeNames[]= { "Nearest", "Linear", "Nearest+Mipmap", "Linear+Mipmap" };
const GLenum g_glWrapModes[]   = { GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
const GLenum g_glMinFilters[]  = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR };
const GLenum g_glMagFilters[]  = { GL_NEAREST, GL_LINEAR, GL_NEAREST, GL_LINEAR };
// Skybox

float skyboxVertices[] =
{
    -1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
    // Right
    1, 2, 6,
    6, 5, 1,
    //Left
    0, 4, 7,
    7, 3, 0,
    //Top
    4, 5, 6,
    6, 7, 4,
    //Bottom
    0, 3, 2,
    2, 1, 0,
    //Back
    0, 1, 5,
    5, 4, 0,
    //Front
    3, 7, 6,
    6, 2, 3
};

int main()
{
    // Set CWD to exe location so relative paths (shaders/, textures/, points/) work
    {
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string exeDir(exePath);
        exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));
        SetCurrentDirectoryA(exeDir.c_str());
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Village fair", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Center the window on the primary monitor so it doesn't open half off-screen
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            if (mode) {
                int mx, my;
                glfwGetMonitorPos(monitor, &mx, &my);
                glfwSetWindowPos(window,
                    mx + (mode->width  - SCR_WIDTH)  / 2,
                    my + (mode->height - SCR_HEIGHT) / 2);
            }
        }
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Keep cursor free so the user can resize/move the window.
    // Press Tab to toggle between free cursor and captured (FPS-look) mode.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/vertexShader.vs", "shaders/fragmentShader.fs");
    Shader lightCubeShader("shaders/lightVertexShader.vs", "shaders/lightFragmentShader.fs");
    Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader instancedShader("shaders/vertexShaderInstanced.vs", "shaders/fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------


    float cube_vertices[] = {
        // positions      // normals         // texture coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, TXmax, TYmax,

        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, TXmin, TYmax,

        0.0f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmin,
        0.0f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, TXmin, TYmin,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, TXmin, TYmax,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, TXmin, TYmin,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmin, TYmin,
        0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, TXmax, TYmin,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmax, TYmax,
        0.0f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, TXmin, TYmax
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };
    //for glass
    float decagon2[] = {
        //0
        0.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // Center point
        -1.0f, 0.0f, -3.0f,.0f, 1.0f, 1.0f, // RED
        1.0f, 0.0f, -3.0f,  .0f, 1.0f, 1.0f, // GREEN
        2.25f, 0.0f, -1.75f, .0f, 1.0f, 1.0f, // BLUE
        3.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // WHITE
        2.25f, 0.0f, 1.75f, .0f, 1.0f, 1.0f, // PINK
        1.0f, 0.0f, 3.0f, .0f, 1.0f, 1.0f, // KHOYERI
        -1.0f, 0.0f, 3.0f, .0f, 1.0f, 1.0f, // SKY BLUE
        -2.25f, 0.0f, 1.75f, .0f, 1.0f, 1.0f, // YELLOW
        -3.0f, 0.0f, 0.0f, .0f, 1.0f, 1.0f, // purple
        -2.25f, 0.0f, -1.75f, .0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,     .469f, .222f, .215f, // Center point
        -1.0f, 1.0f, -3.0f,      .469f, .222f, .215f,  // RED
        1.0f, 1.0f, -3.0f,       .469f, .222f, .215f,  // GREEN
        2.25f, 1.0f, -1.75f,     .469f, .222f, .215f,  // BLUE
        3.0f, 1.0f, 0.0f,     .469f, .222f, .215f, // WHITE
        2.25f, 1.0f, 1.75f,     .469f, .222f, .215f,  // PINK
        1.0f, 1.0f, 3.0f,     .469f, .222f, .215f,  // KHOYERI
        -1.0f, 1.0f, 3.0f,     .469f, .222f, .215f,  // SKY BLUE
        -2.25f, 1.0f, 1.75f,    .469f, .222f, .215f, // YELLOW
        -3.0f, 1.0f, 0.0f,      .469f, .222f, .215f,// purple
        -2.25f, 1.0f, -1.75f,     .469f, .222f, .215f,
        //22
        -4.0f, 0.0f, -4.0f, 1.0f, 1.0f, 1.0f,
        4.0f, 0.0f, -4.0f,  1.0f, 1.0f, 1.0f,
        4.0f, 0.0f, 4.0f, 1.0f, 1.0f, 1.0f,
        -4.0f, 0.0f, 4.0f, 1.0f, 1.0f, 1.0f, // WHITE
    };
    unsigned int deca2_indices[] = {
        0,1,2,
        0,2,3,
        0,3,4,
        0,4,5,
        0,5,6,
        0,6,7,
        0,7,8,
        0,8,9,
        0,9,10,
        0,10,1,

        /*11,12,13,
        11,13,14,
        11,14,15,
        11,15,16,
        11,16,17,
        11,17,18,
        11,18,19,
        11,19,20,
        11,20,21,
        11,21,12,*/


        2,12,13,
        2,12,1,

        3,13,14,
        3,13,2,

        4,14,15,
        4,14,3,

        5,15,16,
        5,15,4,

        6,16,17,
        6,16,5,

        7,17,18,
        7,17,6,

        8,18,19,
        8,18,7,

        9,19,20,
        9,19,8,

        10,20,21,
        10,20,9,

        1,21,12,
        1,21,10,

        ///
        24,22,23,
        24,22,25

    };


    // Initialize sphere model data
    generateSphere(1.0f, 24, 18);

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(2);

    //light's VAO
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //for decagon2
    unsigned int VBOdec2, VAOdec2, EBOdec2;
    glGenVertexArrays(1, &VAOdec2);
    glGenBuffers(1, &VBOdec2);
    glGenBuffers(1, &EBOdec2);
    glBindVertexArray(VAOdec2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOdec2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decagon2), decagon2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOdec2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(deca2_indices), deca2_indices, GL_STATIC_DRAW);
    // position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(1);

    //For Skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //Day
    string facesCubemapDay[6] = {
        "textures/back1.png",
        "textures/left1.png",
        "textures/top1.png",
        "textures/bottom1.png",
        "textures/right1.png",
        "textures/left12.png"
    };

    unsigned int cubemapTextureDay;
    glGenTextures(1, &cubemapTextureDay);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureDay);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemapDay[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        }
        else
            cout << "Failed to load texture: " << facesCubemapDay[i] << endl;
        stbi_image_free(data);
    }

    //Night
    string facesCubemapNight[6] = {
        "textures/night_right.jpeg",
        "textures/night_left.jpeg",
        "textures/night_top.jpeg",
        "textures/night_bottom.jpeg",
        "textures/night_front.jpeg",
        "textures/night_back.jpeg"
    };

    unsigned int cubemapTextureNight;
    glGenTextures(1, &cubemapTextureNight);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemapNight[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        }
        else
            cout << "Failed to load texture: " << facesCubemapNight[i] << endl;
        stbi_image_free(data);
    }




    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




    // Texture loading

    //string footballPath = "ballprint.png";
    // Texture ID for water
    // Texture ID for water
    /*unsigned int waterTextureID;

    // Load water texture
    glGenTextures(1, &waterTextureID);
    glBindTexture(GL_TEXTURE_2D, waterTextureID);

    // Set texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image data
    int width, height;
    unsigned char* data = stbi_load("blue.jpg", &width, &height, nullptr, 3); // Force RGB format
    if (data) {
        // Use RGB format for the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Water texture loaded: " << width << "x" << height << " pixels" << std::endl;
    }
    else {
        std::cerr << "Failed to load water texture!" << std::endl;
    }
    stbi_image_free(data); // Free image memory

    */
    load_texture(texture0, "textures/transparent.png", GL_RGBA);
    load_texture(texture1, "textures/grass_2.png", GL_RGBA);
    load_texture(texture2, "textures/brickwall_1.jpg", GL_RGB);
    load_texture(texture3, "textures/wallside1.png", GL_RGBA);
    load_texture(texture4, "textures/road1.png", GL_RGBA);
    load_texture(texture5, "textures/concrete1.png", GL_RGBA);
    load_texture(texture6, "textures/concrete2.png", GL_RGBA);
    load_texture(texture7, "textures/concrete3.png", GL_RGBA);
    load_texture(texture10, "textures/leather2.png", GL_RGBA);
    load_texture(texture8, "textures/ballprint.png", GL_RGBA);
    load_texture(garden2, "textures/flower1.png", GL_RGBA);
    load_texture(garden1, "textures/flower2.png", GL_RGBA);
    load_texture(whiteflower, "textures/white.png", GL_RGBA);
    load_texture(shoptex, "textures/shop.png", GL_RGBA);
    load_texture(bambooTex, "textures/bamboo.png", GL_RGBA);
    load_texture(barkTex,   "textures/bark.jpg",        GL_RGB);
    load_texture(leafTex,   "textures/leaf.png",        GL_RGBA);
    load_texture(brickTex2, "textures/brickwall_2.jpg", GL_RGB);
    load_texture(signTex,   "textures/village_fare.png", GL_RGBA);

    // Shop item billboard textures (RGBA with alpha, CLAMP_TO_EDGE)
    auto loadItemTex = [](unsigned int& tex, const char* path) {
        load_texture(tex, path, GL_RGBA);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    };
    loadItemTex(itemChipsTex,       "textures/items/chips.png");
    loadItemTex(itemPopcornTex,     "textures/items/popcorn.png");
    loadItemTex(itemSlushyTex,      "textures/items/slushy.png");
    loadItemTex(itemTeddyTex,       "textures/items/teddybearwhite.png");
    loadItemTex(itemTeddyMultiTex,  "textures/items/teddybear_multiple.png");
    loadItemTex(itemCottonPinkTex,  "textures/items/cotton_candy_pink.png");
    loadItemTex(itemCottonBlueTex,  "textures/items/cottoncandy_blue.png");


    //unsigned int footballMap = loadTexture(footballPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);




    Sphere lamp = Sphere();
    SphereWithTexture ball1 = SphereWithTexture(0.25f, 144, 44, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f, texture8, texture8, 0.0f, 0.0f, 1.0f, 1.0f);
  
    //*******************************Curve***************************

    read_file("points/ferris_wheel_points.txt", cntrlPoints);
    bezierVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1, coordinates, normals, indices, vertices);

    read_file("points/boat_points.txt", cntrlPointsBoat);
    boatVAO = hollowBezier(cntrlPointsBoat.data(), ((unsigned int)cntrlPointsBoat.size() / 3) - 1, coordinatesBoat, normalsBoat, indicesBoat, verticesBoat);

    read_file("points/cylinder_points.txt", cntrlPointsCylinder);
    cylinderVAO = hollowBezier(cntrlPointsCylinder.data(), ((unsigned int)cntrlPointsCylinder.size() / 3) - 1, coordinatesCylinder, normalsCylinder, indicesCylinder, verticesCylinder);

    read_file("points/carousal_points.txt", cntrlPointsCarousal);
    carousalVAO = hollowBezier(cntrlPointsCarousal.data(), ((unsigned int)cntrlPointsCarousal.size() / 3) - 1, coordinatesCarousal, normalsCarousal, indicesCarousal, verticesCarousal);

    // Generate geometry for the river
    riverVAO = hollowBezier(
        riverControlPoints.data(),
        static_cast<int>(riverControlPoints.size() / 3) - 1, // Number of control segments
        riverCoordinates,
        riverNormals,
        riverIndices,
        riverVertices
    );
    std::vector<float> trunkControlPoints = generateTrunkControlPoints();
std::vector<float> canopyControlPoints = generateCanopyControlPoints();

unsigned int trunkVAO = hollowBezier(
    trunkControlPoints.data(),
    static_cast<int>(trunkControlPoints.size() / 3) - 1,
    trunkCoordinates,
    trunkNormals,
    trunkIndices,
    trunkVertices
);

unsigned int canopyVAO = hollowBezier(
    canopyControlPoints.data(),
    static_cast<int>(canopyControlPoints.size() / 3) - 1,
    canopyCoordinates,
    canopyNormals,
    canopyIndices,
    canopyVertices
);

    // Build banyan tree geometry (done once before the render loop)
    BanyanTree banyanTree;
    banyanTree.build();

    // Build instanced mini-banyan forest (800 trees outside fairground)
    MiniBanyanForest miniBanyanForest;
    miniBanyanForest.build(22.0f, 200.0f, 800);

    // Build circus tent (uses global circusTent)
    circusTent.build();

    // Position the tent — bottom-left cell of 2x2 center grid
    const glm::vec3 tentWorldPos = glm::vec3(-5.0f, -0.42f, -3.0f);
    const glm::mat4 tentModel    = glm::translate(glm::mat4(1.0f), tentWorldPos);

    // Reposition the 4 point lights to the tent corner pole tops
    {
        glm::vec3 lamps[4];
        circusTent.getLampPositions(tentModel, lamps);
        pointLight1.position = lamps[0];
        pointLight2.position = lamps[1];
        pointLight3.position = lamps[2];
        pointLight4.position = lamps[3];
        // Warm yellow-white, strong enough to light the tent interior
        glm::vec4 lampAmb(0.40f, 0.35f, 0.20f, 1.0f);
        glm::vec4 lampDiff(1.0f,  0.90f, 0.60f, 1.0f);
        glm::vec4 lampSpec(0.8f,  0.80f, 0.60f, 1.0f);
        for (auto* pl : {&pointLight1, &pointLight2, &pointLight3, &pointLight4}) {
            pl->ambient  = lampAmb;
            pl->diffuse  = lampDiff;
            pl->specular = lampSpec;
            pl->Kc = 1.0f; pl->Kl = 0.14f; pl->Kq = 0.07f;
        }
        // Turn them on
        pointLightOn[0] = pointLightOn[1] = pointLightOn[2] = pointLightOn[3] = 1.0f;
    }

    // render loop
    // -----------
    initPondShip(); // Initialize the bezier pond + pirate ship mesh
    // Initialize AABB collision boxes for world objects
    initWorldColliders();

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Animate circus tent door & spotlights
        circusTent.updateDoor(deltaTime);
        circusTent.updateSpotlights(deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // activate shader
        ourShader.use();
        ourShader.setBool("alphaTest", false); // default off; BanyanTree enables it only for leaves
        glBindVertexArray(cubeVAO);

        // ── Projection: wider FOV for immersive feel, adjustable via scroll ──
        glm::mat4 projection = glm::perspective(glm::radians(g_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        ourShader.setMat4("projection", projection);

        // ── Player-driven camera ──
        float yawR   = glm::radians(g_playerYaw);
        float pitchR = glm::radians(g_playerPitch);
        glm::vec3 playerFront;
        playerFront.x = cos(yawR) * cos(pitchR);
        playerFront.y = sin(pitchR);
        playerFront.z = sin(yawR) * cos(pitchR);
        playerFront = glm::normalize(playerFront);

        // Eye height
        const float EYE_HEIGHT = 1.6f;
        glm::vec3 eyePos = g_playerPos + glm::vec3(0.0f, EYE_HEIGHT, 0.0f);

        glm::mat4 view;
        if (g_camMode == CAM_FIRST_PERSON) {
            // FPV: camera at eye level, slight forward offset to avoid head clipping
            glm::vec3 fpvPos = eyePos + playerFront * 0.15f;
            // Walking bob
            float bob = 0.0f;
            if (g_playerMoving)
                bob = sinf(g_walkTimer) * 0.04f;
            fpvPos.y += bob;
            view = glm::lookAt(fpvPos, fpvPos + playerFront, glm::vec3(0,1,0));
            // Update camera object so skybox and other systems pick up the position
            camera.Position = fpvPos;
        } else {
            // Chase camera: behind and above the player with lerp smoothing
            // Inside tent: pull camera much closer so it stays within the walls
            float chaseDist   = g_insideTent ? 1.2f : 3.5f;
            float chaseHeight = g_insideTent ? 0.6f : 1.8f;
            glm::vec3 flatFront(cos(yawR), 0.0f, sin(yawR));
            glm::vec3 desiredPos = eyePos - flatFront * chaseDist + glm::vec3(0.0f, chaseHeight, 0.0f);
            // Smooth follow (lerp)
            float t = 1.0f - expf(-g_chaseSmoothSpeed * deltaTime);
            g_chaseCamPos = glm::mix(g_chaseCamPos, desiredPos, t);
            // LookAt target: ahead of player in their look direction (includes pitch)
            glm::vec3 lookTarget = eyePos + playerFront * 3.0f;
            view = glm::lookAt(g_chaseCamPos, lookTarget, glm::vec3(0,1,0));
            camera.Position = g_chaseCamPos;
        }
        camera.Front = playerFront;
        camera.LOOKAT = camera.Position + camera.Front;
        ourShader.setMat4("view", view);

        
        //**************lighting**************

        // === SUN: time-of-day driven directional light ===
        // Full day cycle every 120 seconds. Sun rises (+X), peaks (+Y), sets (-X).
        {
            // 10 Q-steps: 0..7 = day (8 steps), 8..9 = night (2 steps)
            // Day arc:   15->165 deg (sun clearly above horizon)
            // Night arc: 210->330 deg (around midnight, skips deep dusk/dawn)
            const float kSunPI = 3.14159265f;
            const float DEG = kSunPI / 180.0f;
            float theta;
            if (g_sunStep <= 7)
                theta = (15.0f + (float)g_sunStep * (150.0f / 7.0f)) * DEG;
            else
                theta = (210.0f + (float)(g_sunStep - 8) * 120.0f) * DEG;

            glm::vec3 sunDir = glm::normalize(glm::vec3(cos(theta), sin(theta), 0.15f));
            float h = sunDir.y; // -1..1, sun height above horizon

            glm::vec3 noonColor  = glm::vec3(1.00f, 0.98f, 0.92f);
            glm::vec3 duskColor  = glm::vec3(1.00f, 0.50f, 0.20f);
            glm::vec3 nightColor = glm::vec3(0.05f, 0.07f, 0.15f);

            float dayBlend  = glm::clamp(h, 0.0f, 1.0f);
            float duskBlend = glm::clamp(1.0f - fabs(h) * 2.5f, 0.0f, 1.0f);
            glm::vec3 sunColor = glm::mix(nightColor, duskColor, duskBlend);
            sunColor = glm::mix(sunColor, noonColor, dayBlend);

            // Stronger diffuse + lower ambient => visible shaded sides / clearer "shadows".
            float intensity = glm::clamp(h + 0.05f, 0.0f, 1.0f);
            directionalLight.direction = -sunDir;
            directionalLight.ambient   = glm::vec4(sunColor * (0.12f + 0.08f * dayBlend), 1.0f);
            directionalLight.diffuse   = glm::vec4(sunColor * intensity * 2.2f, 1.0f);
            directionalLight.specular  = glm::vec4(sunColor * intensity * 0.8f, 1.0f);

            // Remember for drawing the visible sun disk later this frame.
            g_sunDir   = sunDir;
            g_sunColor = sunColor;
        }

        //Setting up Directional Light
        directionalLight.setUpLight(ourShader);
        if (!directionalLightOn)
            directionalLight.turnOff();
        if (!ambientOn)
            directionalLight.turnAmbientOff();
        if(!diffuseOn)
            directionalLight.turnDiffuseOff();
        if(!specularOn)
            directionalLight.turnSpecularOff();

        //Setting up Point Light (tent corners 1-4)
        SetupPointLight(pointLight1, ourShader, 1);
        SetupPointLight(pointLight2, ourShader, 2);
        SetupPointLight(pointLight3, ourShader, 3);
        SetupPointLight(pointLight4, ourShader, 4);
        //Setting up Point Light (lamp posts 5-13)
        SetupPointLight(pointLight5,  ourShader, 5);
        SetupPointLight(pointLight6,  ourShader, 6);
        SetupPointLight(pointLight7,  ourShader, 7);
        SetupPointLight(pointLight8,  ourShader, 8);
        SetupPointLight(pointLight9,  ourShader, 9);
        SetupPointLight(pointLight10, ourShader, 10);
        SetupPointLight(pointLight11, ourShader, 11);
        SetupPointLight(pointLight12, ourShader, 12);
        SetupPointLight(pointLight13, ourShader, 13);

        //Setting up Spot Light — follow camera as flashlight
        spotLight.position = camera.Position;
        spotLight.direction = camera.Front;
        spotLight.setUpLight(ourShader);
        if (!spotLightOn)
            spotLight.turnOff();
        if (!ambientOn)
            spotLight.turnAmbientOff();
        if (!diffuseOn)
            spotLight.turnDiffuseOff();
        if (!specularOn)
            spotLight.turnSpecularOff();

        // ---- Circus spotlight uniforms (6 interior + 6 exterior) ----
        ourShader.setBool("circusLightsOn", true);
        for (int i = 0; i < CircusTent::NUM_SPOTS; ++i) {
            // Interior spotlights [0..5]
            glm::vec3 pos, dir;
            circusTent.getSpotParams(tentModel, i, pos, dir);
            char buf[64];
            snprintf(buf, sizeof buf, "circusSpotLights[%d]", i);
            string base(buf);
            glm::vec3 col = circusTent.spotColors[i];
            ourShader.setVec3(base + ".position", pos);
            ourShader.setVec3(base + ".direction", dir);
            ourShader.setFloat(base + ".cutOff", glm::cos(glm::radians(18.0f)));
            ourShader.setFloat(base + ".outerCutOff", glm::cos(glm::radians(25.0f)));
            ourShader.setVec4(base + ".ambient",  glm::vec4(col * 0.35f, 1.0f));
            ourShader.setVec4(base + ".diffuse",  glm::vec4(col * 1.8f, 1.0f));
            ourShader.setVec4(base + ".specular", glm::vec4(col * 1.2f, 1.0f));
            ourShader.setFloat(base + ".Kc", 1.0f);
            ourShader.setFloat(base + ".Kl", 0.09f);
            ourShader.setFloat(base + ".Kq", 0.05f);

            // Exterior spotlights [6..11] — on anchor posts
            float ang = 6.28318530f * i / 6 + 0.52f;
            float ca = cosf(ang), sa = sinf(ang);
            float ANCHOR_R = CircusTent::R * 1.85f;
            float POST_H   = 0.7f;
            glm::vec3 postTop(ANCHOR_R * ca, POST_H, ANCHOR_R * sa);
            float phase = 6.28318530f * i / 6;
            float tilt  = 0.3f + 0.35f * sinf(circusTent.spotSweep * 1.2f + phase);
            glm::vec3 groundTarget(-ANCHOR_R * 0.3f * ca, -0.3f, -ANCHOR_R * 0.3f * sa);
            glm::vec3 toGround = glm::normalize(groundTarget - postTop);
            glm::vec3 apex(0, CircusTent::WALL_H * 0.5f, 0);
            glm::vec3 toApex = glm::normalize(apex - postTop);
            glm::vec3 aimDir = glm::normalize(glm::mix(toGround, toApex, tilt));
            glm::vec3 wPos = glm::vec3(tentModel * glm::vec4(postTop, 1.0f));
            glm::vec3 wDir = glm::normalize(glm::vec3(tentModel * glm::vec4(aimDir, 0.0f)));

            snprintf(buf, sizeof buf, "circusSpotLights[%d]", i + 6);
            string eb(buf);
            ourShader.setVec3(eb + ".position", wPos);
            ourShader.setVec3(eb + ".direction", wDir);
            ourShader.setFloat(eb + ".cutOff", glm::cos(glm::radians(22.0f)));
            ourShader.setFloat(eb + ".outerCutOff", glm::cos(glm::radians(35.0f)));
            ourShader.setVec4(eb + ".ambient",  glm::vec4(col * 0.10f, 1.0f));
            ourShader.setVec4(eb + ".diffuse",  glm::vec4(col * 0.6f, 1.0f));
            ourShader.setVec4(eb + ".specular", glm::vec4(col * 0.4f, 1.0f));
            ourShader.setFloat(eb + ".Kc", 1.0f);
            ourShader.setFloat(eb + ".Kl", 0.14f);
            ourShader.setFloat(eb + ".Kq", 0.07f);
        }

        //Setting up Camera and Others
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setBool("lightingOn", lightingOn);

        // Emissive glow uniform
        ourShader.setBool("emissiveOn", emissiveOn);
        ourShader.setVec4("material.emissive", glm::vec4(0.0f));  // default off per-object

        // Texture mode uniform
        ourShader.setInt("textureMode", g_textureMode);

        // ── Fog uniforms (main shader) ─────────────────
        ourShader.setBool("fogEnabled", true);
        ourShader.setVec3("fogColor", dark ? glm::vec3(0.02f, 0.03f, 0.06f)
                                           : glm::vec3(0.62f, 0.72f, 0.78f));
        ourShader.setFloat("fogStart", 60.0f);
        ourShader.setFloat("fogEnd",   250.0f);

        //cout << camera.Position[0] << " " << camera.Position[1] << " " << camera.Position[2] << endl;


        // Starting
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, scaleMatrix, rotateMatrix,model;
        glm::vec4 color1;

        float chairZ = 0.4f;
        float fixY = 0.45f;
        float chairX = 0.1f;



        
        //********** Object making ***********


        glActiveTexture(GL_TEXTURE0);

        // === LARGE GROUND PLANE - extends to horizon so village doesn't float ===
        // Cube vertices span (0,0,0) to (0.5,0.5,0.5).
        // With scale 4000, ground spans 0..2000 units.
        // To center at village (Xâ‰ˆ8, Zâ‰ˆ0): translate X = 8 - 2000/2 = -992, Z = 0 - 2000/2 = -1000
        {
            glBindTexture(GL_TEXTURE_2D, texture1);  // grass texture
            glm::mat4 groundTranslate = glm::translate(identityMatrix, glm::vec3(-992.0f, -0.46f, -1000.0f));
            glm::mat4 groundScale = glm::scale(identityMatrix, glm::vec3(4000.0f, 0.1f, 4000.0f));
            glm::mat4 groundModel = groundTranslate * groundScale;
            ourShader.setMat4("model", groundModel);
            // Tile the grass texture across the huge ground so it doesn't stretch into a flat color
            ourShader.setVec2("uvScale", glm::vec2(500.0f, 500.0f));
            ourShader.setVec4("material.ambient", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(0.05f, 0.05f, 0.05f, 0.5f));
            ourShader.setFloat("material.shininess", 4.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            ourShader.setVec2("uvScale", glm::vec2(1.0f, 1.0f));
        }

        // === Visible sun disk (unlit emissive cube, placed far along sun direction) ===
        {
            glm::vec3 sunPos = camera.Position + g_sunDir * 800.0f;
            glm::mat4 sunModel = glm::translate(identityMatrix, sunPos) *
                                 glm::scale(identityMatrix, glm::vec3(70.0f));
            ourShader.setMat4("model", sunModel);
            ourShader.setBool("lightingOn", false);
            glBindTexture(GL_TEXTURE_2D, whiteflower); // plain white texture
            // Brighten when sun is high; dim/tint when setting
            glm::vec4 sunEmissive = glm::vec4(g_sunColor * 1.8f, 1.0f);
            ourShader.setVec4("material.ambient",  sunEmissive);
            ourShader.setVec4("material.diffuse",  sunEmissive);
            ourShader.setVec4("material.specular", glm::vec4(0.0f));
            ourShader.setFloat("material.shininess", 1.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            ourShader.setBool("lightingOn", lightingOn); // restore
        }
        //Ground
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -2.15f, 2.0f));
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (!textureOn) {
            glBindTexture(GL_TEXTURE_2D, texture1);
            Floor(ourShader, translateMatrix, color1);
        }
        else {

            Floor(ourShader, translateMatrix, color1);
        }

        // ================================================================
        //  LAYOUT: 2x2 center grid + shop wings + peripherals
        //
        //  Tent footprint: R=4, ANCHOR_R=R*1.85=7.4  → cell ≥ 15x15
        //
        //  Center rectangle (roads boundary):
        //    X: -14 to +10       Z: -13 to +8
        //  Vertical divider ≈ X=-1    Horiz divider ≈ Z=+1
        //
        //  Cell (1,1) Top-Left    : Banyan Tree   center(-5,  +5)
        //  Cell (1,2) Top-Right   : Carousel       center(+5, +4)
        //  Cell (2,1) Bottom-Left : Circus Tent   center(-5, -4) ← biggest
        //  Cell (2,2) Bottom-Right: Ferris Wheel  center(+5, -4)
        // ================================================================

        // ================================================================
        //  U-SHAPED ROADS around center rectangle
        // ================================================================
        glBindTexture(GL_TEXTURE_2D, texture4);
        {
            glm::vec4 roadColor(0.45f, 0.45f, 0.45f, 1.0f);
            ourShader.setVec4("material.ambient", roadColor);
            ourShader.setVec4("material.diffuse", roadColor);
            ourShader.setVec4("material.specular", glm::vec4(0.1f));
            ourShader.setFloat("material.shininess", 8.0f);

            // LEFT road (along Z): X from -14 to -12, Z from -13 to +18
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.0f, -0.41f, -13.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.05f, 48.0f));
            ourShader.setMat4("model", translateMatrix * scaleMatrix);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // RIGHT road (along Z): X from +10 to +12, Z from -13 to +18
            translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.41f, -13.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(4.0f, 0.05f, 48.0f));
            ourShader.setMat4("model", translateMatrix * scaleMatrix);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // BOTTOM road (along X): Z from -13 to -11, X from -14 to +12
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.0f, -0.41f, -13.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(52.0f, 0.05f, 4.0f));
            ourShader.setMat4("model", translateMatrix * scaleMatrix);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // TOP road (along X): Z from +16 to +18, X from -14 to +12 (closes the square)
            translateMatrix = glm::translate(identityMatrix, glm::vec3(-14.0f, -0.41f, 11.0f));
            scaleMatrix = glm::scale(identityMatrix, glm::vec3(52.0f, 0.05f, 4.0f));
            ourShader.setMat4("model", translateMatrix * scaleMatrix);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // ================================================================
        //  SHOPS — 3 modular rows using shops1/2/3.h
        // ================================================================
        // Left Wing (shops1.h): vertical row along -X, outside left road
        // shops1 places 3 shops at Z offsets 0, -5.5, -11 with rotCW (face +X)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-17.0f, 0.0f, 5.0f));
        drawShopRow1(ourShader, translateMatrix, textureOn);

        // Right Wing (shops2.h): vertical row along +X, outside right road
        // shops2 places 3 shops at Z offsets 0, -5.5, -11 with rotCCW (face -X)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.0f, 0.0f, 5.0f));
        drawShopRow2(ourShader, translateMatrix, textureOn);

        // Bottom Wing (shops3.h): horizontal row along -Z, outside bottom road
        // shops3 places 3 shops at X offsets 0, 6, 12 with rot180 (face +Z)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.0f, 0.0f, -16.0f));
        drawShopRow3(ourShader, translateMatrix, textureOn);

        // ================================================================
        //  UMBRELLAS — between shops in each row
        // ================================================================
        glBindTexture(GL_TEXTURE_2D, texture0);
        // Left side (between shops at Z=5, -0.5, -6)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-17.5f, -0.42f, 2.25f));
        drawUmbrella(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-17.5f, -0.42f, -3.0f));
        drawUmbrella(ourShader, translateMatrix);
        // Right side (between shops at Z=5, -0.5, -6)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.5f, -0.42f, 2.25f));
        drawUmbrella(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(15.5f, -0.42f, -3.0f));
        drawUmbrella(ourShader, translateMatrix);
        // Bottom (between shops at X=-7, -1, 5)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-4.0f, -0.42f, -16.5f));
        drawUmbrella(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, -0.42f, -16.5f));
        drawUmbrella(ourShader, translateMatrix);

        // ================================================================
        //  CIRCUS TENT — Cell (2,1) Bottom-Left, center (-5, -4)
        //  Tent R=4, ANCHOR_R=7.4 → fits in cell X[-14,-1] Z[-13,+1]
        // ================================================================
        circusTent.draw(ourShader, tentModel, whiteflower, lightingOn);
        glBindVertexArray(cubeVAO);

        // ================================================================
        //  BANYAN TREE — Cell (1,1) Top-Left, center (-5, +5)
        // ================================================================
        {
            const glm::vec3 banyanPos   = glm::vec3(-4.0f, -0.42f, 7.0f);
            const float     banyanScale = 1.5f;
            glm::mat4 banyanModel =
                glm::translate(identityMatrix, banyanPos) *
                glm::scale(identityMatrix, glm::vec3(banyanScale));
            ourShader.setBool("lightingOn", lightingOn);
            banyanTree.drawPlatform(ourShader, banyanPos, banyanScale, brickTex2);
            banyanTree.draw(ourShader, banyanModel, barkTex, leafTex);
            glBindVertexArray(cubeVAO);
        }

        // ================================================================
        //  RIDES
        // ================================================================
        // Carousel — Cell (1,2) Top-Right, world center (+5, +4)
        // Internal pivot: (14, 1.2, -4.25) → offset: (5-14, 0, 4-(-4.25))
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-9.0f, 0.0f, 8.25f));
        Carousal(ourShader, translateMatrix, rotateAngleTest_Y);

        // Ferris Wheel — Cell (2,2) Bottom-Right, world center (+5, -4)
        // Internal center: (3, 2, 11.5) → offset: (5-3, 0, -4-11.5)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.0f, -17.5f));
        FerrisWheel(ourShader, translateMatrix);

        // ================================================================
        //  POND + PIRATE SHIP — top edge (+Z), centered between shops
        // ================================================================
        {
            float currentTime = (float)glfwGetTime();
            glm::mat4 pondMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.3f, 20.0f));
            drawPondShipScene(ourShader, pondMatrix, currentTime);
            glBindVertexArray(cubeVAO);
        }

        // ================================================================
        //  BENCHES & CHAIRS — top corners flanking pond
        // ================================================================
        // (Chairs moved into drawUmbrella)

        // ================================================================
        //  LAMP POSTS — along inner edges of roads
        //  Each drawLampPost call draws the stand AND its glowing bulb
        //  sphere at the arm tip, so no separate bulb placement is needed.
        // ================================================================
        glBindTexture(GL_TEXTURE_2D, texture0);
        // Left road inner edge (X≈-12)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-12.0f, -0.42f, 5.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-12.0f, -0.42f, -1.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-12.0f, -0.42f, -7.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        // Right road inner edge (X≈+10)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.42f, 5.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.42f, -1.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.0f, -0.42f, -7.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        // Bottom road inner edge (Z≈-11)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-6.0f, -0.42f, -11.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.42f, -11.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, -0.42f, -11.0f));
        drawLampPost(ourShader, lightCubeShader, lamp, view, projection, translateMatrix);

        // ================================================================
        //  FENCE BOUNDARY — grounded posts, rope, bunting + entrance gate
        // ================================================================
        {
            float currentTime = (float)glfwGetTime();
            drawFenceBoundary(ourShader, identityMatrix,
                              bambooTex, texture0, signTex, currentTime);
            glBindVertexArray(cubeVAO);
        }

        // ================================================================
        //  PLAYER CHARACTER
        // ================================================================
        {
            ourShader.use();
            float walkPhase = g_playerMoving ? sinf(g_walkTimer) : 0.0f;
            if (g_camMode == CAM_CHASE) {
                // Third-person: draw full body at player position, rotated to face yaw
                glm::mat4 playerRoot = glm::translate(glm::mat4(1.0f), g_playerPos)
                    * glm::rotate(glm::mat4(1.0f), glm::radians(g_playerYaw - 90.0f), glm::vec3(0,1,0));
                drawPlayerThirdPerson(ourShader, playerRoot, texture0, walkPhase);
            } else {
                // First-person: draw arms as viewmodel (in camera-relative space)
                glm::mat4 invView = glm::inverse(view);
                drawPlayerFirstPerson(ourShader, invView, texture0, walkPhase);
            }
            glBindVertexArray(cubeVAO);
        }

        // ================================================================
        //  MISC PROPS
        // ================================================================
        // Balloon vendor near entry (bottom-left corner)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-10.0f, -0.42f, -14.0f));
        BalloonBunch(ourShader, translateMatrix);

        // Vendor cart in center area
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, -0.42f, 1.0f));
        VendorCart(ourShader, translateMatrix);

        // Well near vertical road gap
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, -0.42f, -5.0f));
        Well(ourShader, translateMatrix);

        // Food place
        glBindTexture(GL_TEXTURE_2D, texture0);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 0.0f, -1.0f));
        FoodPlace(ourShader, translateMatrix, rotateAngleTest_Y);

        // Kite jar
        glm::mat4 jarMatrix = glm::translate(identityMatrix, glm::vec3(-3.0f, -0.2f, 1.0f));
        drawJar(ourShader, jarMatrix, glm::vec4(0.8f, 0.5f, 0.3f, 1.0f), 0.5f, 0.5f, 36);
        for (int i = 0; i < 8; ++i) {
            float angle = glm::radians(i * 360.0f / 8);
            glm::vec3 rp(0.25f * cos(angle), 0.1f + 0.2f * (i % 3), 0.25f * sin(angle));
            glm::mat4 kiteMatrix = jarMatrix * glm::translate(glm::mat4(1.0f), rp);
            drawKiteWithStick(ourShader, kiteMatrix,
                glm::vec4(1.0f - 0.1f * i, 0.1f * i, 0.5f, 1.0f),
                glm::vec4(0.3f, 0.2f, 0.1f, 1.0f), 0.5f, glm::vec3(0.0f));
        }

        // Decorative balls
        translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, -0.15f, 3.0f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.15f, 3.0f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.7f, -0.15f, 2.5f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);

        // Decorative trees near pond
        glm::mat4 treeModel = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 9.0f));
        drawRealisticTree(ourShader, treeModel, trunkVAO, canopyVAO, trunkIndices, canopyIndices);
        treeModel = glm::translate(glm::mat4(1.0f), glm::vec3(9.0f, 0.0f, 9.0f));
        drawRealisticTree(ourShader, treeModel, trunkVAO, canopyVAO, trunkIndices, canopyIndices);

        // ================================================================
        //  INSTANCED MINI-BANYAN FOREST — outside fairground perimeter
        // ================================================================
        {
            instancedShader.use();
            instancedShader.setMat4("view", view);
            instancedShader.setMat4("projection", projection);

            // Copy lighting uniforms to instanced shader
            instancedShader.setVec3("viewPos", camera.Position);
            instancedShader.setBool("lightingOn", lightingOn);

            // Fog
            instancedShader.setBool("fogEnabled", true);
            instancedShader.setVec3("fogColor", dark ? glm::vec3(0.02f, 0.03f, 0.06f)
                                                     : glm::vec3(0.62f, 0.72f, 0.78f));
            instancedShader.setFloat("fogStart", 40.0f);
            instancedShader.setFloat("fogEnd",   180.0f);

            // Directional light for forest
            directionalLight.setUpLight(instancedShader);
            // Point lights (minimal influence at distance, but set for correctness)
            SetupPointLight(pointLight1, instancedShader, 1);
            SetupPointLight(pointLight2, instancedShader, 2);
            SetupPointLight(pointLight3, instancedShader, 3);
            SetupPointLight(pointLight4, instancedShader, 4);
            // Spot light
            spotLight.setUpLight(instancedShader);
            instancedShader.setBool("circusLightsOn", false);

            miniBanyanForest.draw(instancedShader, barkTex, leafTex);

            // Switch back to main shader
            ourShader.use();
            glBindVertexArray(cubeVAO);
        }

        //********* END of Object Making **********

        // Lights
        //
        // The lamppost bulbs are drawn by drawLampPost() itself, so we
        // only emit a visualization sphere for the spot light (index 5).
        // The stale lightPositions[1..4] point-light spheres were removed
        // — they used hardcoded positions that did not match anything in
        // the scene and visually looked like displaced bulbs on the ground.
        lightCubeShader.use();
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.13f, 0.1f, 0.13f));
        {
            glm::vec4 bodyColor = (spotLightOn == 0.0f)
                ? glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
                : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            lightCubeShader.setVec4("bodyColor", bodyColor);
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            lightCubeShader.setMat4("projection", projection);
            glm::mat4 view = camera.GetViewMatrix();
            lightCubeShader.setMat4("view", view);
            glm::mat4 tempModel = glm::translate(identityMatrix, lightPositions[5]);
            lightCubeShader.setMat4("model", tempModel * scaleMatrix);
            lamp.drawSphere(lightCubeShader);
        }


        //Texture drawing
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();
        glm::mat4 view1 = glm::mat4(1.0f);
        glm::mat4 projection1 = glm::mat4(1.0f);
        view1 = glm::mat4(glm::mat3(view));  // strip translation from scene view matrix
        projection1 = glm::perspective(glm::radians(g_fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        skyboxShader.setMat4("view", view1);

        // Horizon fog � haze the base of the skybox to blend with scene fog
        skyboxShader.setBool("horizonFogEnabled", true);
        skyboxShader.setVec3("horizonFogColor", dark ? glm::vec3(0.02f, 0.03f, 0.06f)
                                                     : glm::vec3(0.62f, 0.72f, 0.78f));
        skyboxShader.setFloat("horizonFogStart", 0.12f);  // Y where haze begins
        skyboxShader.setFloat("horizonFogEnd",  -0.02f);  // Y where haze is fully opaque
        skyboxShader.setMat4("projection", projection1);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        if (dark)
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
        }
        else
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureDay);
        }
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

// ================================================================
// Segmented source files (#included for readability):
// ================================================================
#include "src/callbacks.h"
#include "src/draw_shop_items.h"
#include "src/draw_static.h"
#include "src/bezier_utils.h"
#include "src/draw_animated.h"
#include "src/draw_pond_ship.h"
#include "src/shops1.h"
#include "src/shops2.h"
#include "src/shops3.h"
#include "src/draw_umbrella.h"
#include "src/draw_boundary.h"
#include "src/draw_player.h"

