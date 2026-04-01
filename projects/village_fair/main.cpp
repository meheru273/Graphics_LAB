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
#include "stb_image.h"

#include <iostream>
#include <stdlib.h>
#include <windows.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void Table(Shader ourshader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void Chair(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void Floor(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void drawCube(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest = 0, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float spec = 1.0f, float shininess = 32.0f);
void drawCube1(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest = 0, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float spec = 1.0f, float shininess = 32.0f);
void Road(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Boundary(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Gate(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color);
void Shop(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 color, unsigned int texture);
void ShopVariant(Shader ourShader, glm::mat4 moveMatrix, glm::vec4 roofTint, unsigned int texture);
void Tree(Shader ourShader, glm::mat4 moveMatrix, float trunkHeight, glm::vec4 leafColor);
void BalloonBunch(Shader ourShader, glm::mat4 moveMatrix);
void FlagBunting(Shader ourShader, glm::mat4 moveMatrix, float length);
void VendorCart(Shader ourShader, glm::mat4 moveMatrix);
void Well(Shader ourShader, glm::mat4 moveMatrix);
void LampPost(Shader ourShader, glm::mat4 moveMatrix);
void load_texture(unsigned int& texture, string image_name, GLenum format);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void SetupPointLight(PointLight &pointLight, Shader ourShader, int lightNum);
void Stage(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void FoodPlace(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void TableChairSet(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Y = 0);
void read_file(string file_name, vector<float> &vec);
long long nCr(int n, int r);
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L);
unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices);
void drawCurve(Shader ourShader, glm::mat4 moveMatrix, unsigned int VAO, vector<int> indices, glm::vec4 color = glm::vec4(1.0f), float rotateAngleTest_Y = 0);
void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix);
void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z = 0);
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


// camera               8.0   1.0   18.1
Camera camera(glm::vec3(8.0f, 1.0f, 18.1f));
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
const int noOfPointLights = 4;

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
bool dark = false;

float directionalLightOn = 1.0;
float pointLightOn[noOfPointLights] = { 0.0, 0.0, 0.0, 0.0 };
float spotLightOn = 0.0;


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

bool textureOn = false;
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
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
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

    // render loop
    // -----------
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

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // activate shader
        ourShader.use();
        glBindVertexArray(cubeVAO);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        ////testing rotating around lookat point
        //const float radius = 2.0f;
        //float camX = sin(glfwGetTime()) * radius;
        //float camZ = cos(glfwGetTime()) * radius;
        //glm::mat4 view;
        //view = glm::lookAt(glm::vec3(camX, 1.0, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        // camera/view transformation
        //glm::mat4 view = basic_camera.createViewMatrix();
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        
        //**************lighting**************

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

        //Setting up Point Light
        SetupPointLight(pointLight1, ourShader, 1);
        SetupPointLight(pointLight2, ourShader, 2);
        SetupPointLight(pointLight3, ourShader, 3);
        SetupPointLight(pointLight4, ourShader, 4);

        //Setting up Spot Light
        spotLight.setUpLight(ourShader);
        if (!spotLightOn)
            spotLight.turnOff();
        if (!ambientOn)
            spotLight.turnAmbientOff();
        if (!diffuseOn)
            spotLight.turnDiffuseOff();
        if (!specularOn)
            spotLight.turnSpecularOff();

        //Setting up Camera and Others
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setBool("lightingOn", lightingOn);

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
        // To center at village (X≈8, Z≈0): translate X = 8 - 2000/2 = -992, Z = 0 - 2000/2 = -1000
        {
            glBindTexture(GL_TEXTURE_2D, texture1);  // grass texture
            glm::mat4 groundTranslate = glm::translate(identityMatrix, glm::vec3(-992.0f, -0.46f, -1000.0f));
            glm::mat4 groundScale = glm::scale(identityMatrix, glm::vec3(4000.0f, 0.1f, 4000.0f));
            glm::mat4 groundModel = groundTranslate * groundScale;
            ourShader.setMat4("model", groundModel);
            ourShader.setVec4("material.ambient", glm::vec4(0.3f, 0.5f, 0.2f, 1.0f));
            ourShader.setVec4("material.diffuse", glm::vec4(0.3f, 0.5f, 0.2f, 1.0f));
            ourShader.setVec4("material.specular", glm::vec4(0.05f, 0.05f, 0.05f, 0.5f));
            ourShader.setFloat("material.shininess", 4.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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

        translateMatrix = glm::translate(identityMatrix, glm::vec3(9.2f, -2.145f, 2.0f));
        color1 = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, texture4);
        Road(ourShader, translateMatrix, color1);

        // Boundary, Garden, and Gate removed

        glm::mat4 rotCW  = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0,1,0));
        glm::mat4 rotCCW = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0,1,0));
        glm::mat4 rot180 = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0,1,0));

        // ---- LEFT SIDE SHOPS (facing road/center) ----
        // Shop 1 - Left front
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, 0.0f, 2.0f));
        Shop(ourShader, translateMatrix * rotCW, glm::vec4(1,1,1,1), textureOn ? texture0 : bambooTex);

        // Shop 2 - Left middle (variant - fruit stall)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, 0.0f, -3.5f));
        ShopVariant(ourShader, translateMatrix * rotCW,
            glm::vec4(0.85f, 0.55f, 0.10f, 1.0f),  // orange roof
            textureOn ? texture0 : bambooTex);

        // Shop 3 - Left back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.0f, 0.0f, -9.0f));
        Shop(ourShader, translateMatrix * rotCW, glm::vec4(0.9f, 0.85f, 0.75f, 1.0f),
            textureOn ? texture0 : bambooTex);

        // ---- RIGHT SIDE SHOPS (facing road/center) ----
        // Shop 4 - Right front
        translateMatrix = glm::translate(identityMatrix, glm::vec3(17.0f, 0.0f, 2.0f));
        Shop(ourShader, translateMatrix * rotCCW, glm::vec4(1,1,1,1), textureOn ? texture0 : bambooTex);

        // Shop 5 - Right middle (variant)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(17.0f, 0.0f, -3.5f));
        ShopVariant(ourShader, translateMatrix * rotCCW,
            glm::vec4(0.60f, 0.80f, 0.40f, 1.0f),  // green roof
            textureOn ? texture0 : bambooTex);

        // Shop 6 - Right back
        translateMatrix = glm::translate(identityMatrix, glm::vec3(17.0f, 0.0f, -9.0f));
        Shop(ourShader, translateMatrix * rotCCW, glm::vec4(0.95f, 0.90f, 0.80f, 1.0f),
            textureOn ? texture0 : bambooTex);

        // Shop 7 - Back wall, facing entrance
        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f, 0.0f, -14.5f));
        ShopVariant(ourShader, translateMatrix * rot180,
            glm::vec4(0.70f, 0.30f, 0.20f, 1.0f),  // red roof
            textureOn ? texture0 : bambooTex);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0f, 0.0f, -14.5f));
        Shop(ourShader, translateMatrix * rot180, glm::vec4(1,1,1,1),
            textureOn ? texture0 : bambooTex);

        // ---- TREES scattered around ----
        glm::vec4 darkGreen   = glm::vec4(0.10f, 0.40f, 0.12f, 1.0f);
        glm::vec4 lightGreen  = glm::vec4(0.25f, 0.55f, 0.18f, 1.0f);

        // Left side tree line
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.42f,  5.0f));
        Tree(ourShader, translateMatrix, 1.4f, darkGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.42f, -1.0f));
        Tree(ourShader, translateMatrix, 1.8f, lightGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.42f, -7.0f));
        Tree(ourShader, translateMatrix, 1.3f, darkGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.5f, -0.42f,-12.0f));
        Tree(ourShader, translateMatrix, 1.6f, lightGreen);

        // Right side tree line
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.5f, -0.42f,  5.0f));
        Tree(ourShader, translateMatrix, 1.5f, lightGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.5f, -0.42f, -1.0f));
        Tree(ourShader, translateMatrix, 1.7f, darkGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.5f, -0.42f, -7.0f));
        Tree(ourShader, translateMatrix, 1.4f, lightGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(19.5f, -0.42f,-12.0f));
        Tree(ourShader, translateMatrix, 1.9f, darkGreen);

        // Front border trees
        for (float fx = -1.0f; fx <= 17.0f; fx += 4.5f) {
            if (abs(fx - 7.3f) < 2.0f) continue; // Skip road
            translateMatrix = glm::translate(identityMatrix, glm::vec3(fx, -0.42f, 16.0f));
            Tree(ourShader, translateMatrix, 1.4f + (0.1f * ((int)fx % 3)), (int)fx % 2 == 0 ? darkGreen : lightGreen);
        }

        // Back border trees
        for (float bx = -1.0f; bx <= 17.0f; bx += 4.5f) {
            translateMatrix = glm::translate(identityMatrix, glm::vec3(bx, -0.42f, -17.0f));
            Tree(ourShader, translateMatrix, 1.5f + (0.1f * ((int)bx % 3)), (int)bx % 2 != 0 ? darkGreen : lightGreen);
        }

        // Scattered interior trees (between attractions)
        translateMatrix = glm::translate(identityMatrix, glm::vec3( 3.5f, -0.42f, 10.0f));
        Tree(ourShader, translateMatrix, 1.2f, darkGreen);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(13.0f, -0.42f, 10.0f));
        Tree(ourShader, translateMatrix, 1.3f, lightGreen);
        // Balloon vendor near entrance
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.5f, -0.42f, 13.0f));
        BalloonBunch(ourShader, translateMatrix);

        glm::mat4 rotY90 = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0,1,0));

        // Across entrance (spans X from gate pillar to gate pillar ~7.0 to 10.0 = 3 units)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f, -0.42f, 15.0f));
        FlagBunting(ourShader, translateMatrix, 3.0f);

        // Left side - runs along Z axis so needs rotY90
        // Starts at front (Z=13) runs toward back (Z=-13) = 26 units
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.8f, -0.42f, 13.0f));
        FlagBunting(ourShader, translateMatrix * rotY90, 26.0f);

        // Right side - same but on right
        translateMatrix = glm::translate(identityMatrix, glm::vec3(18.0f, -0.42f, 13.0f));
        FlagBunting(ourShader, translateMatrix * rotY90, 26.0f);

        // Cross bunting over road center (optional - festive arch effect)
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f, -0.42f, 5.0f));
        FlagBunting(ourShader, translateMatrix, 3.0f);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.0f, -0.42f, -5.0f));
        FlagBunting(ourShader, translateMatrix, 3.0f);

        // Vendor cart in center path
        translateMatrix = glm::translate(identityMatrix, glm::vec3(7.5f, -0.42f, 5.0f));
        VendorCart(ourShader, translateMatrix);

        // Well near back of fair
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, -0.42f, -8.0f));
        Well(ourShader, translateMatrix);
        //seat
        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, -0.6f, 0.0f));
        seat(ourShader, translateMatrix);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0f, -0.6f, 0.0f));
        seat(ourShader, translateMatrix);


        
        // Food place
        glBindTexture(GL_TEXTURE_2D, texture0);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        FoodPlace(ourShader, translateMatrix, rotateAngleTest_Y);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-3.13f, 0.0f, -6.93f));
        LampPost(ourShader, translateMatrix);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(5.0f, 1.2f, 8.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.6f, 0.6f));
        drawCube1(ourShader, translateMatrix * scaleMatrix, 60.0f, glm::vec4(0.2f));

        
        // Draw the jar
        glm::mat4 jarMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, -0.2f, 3.0f));
        drawJar(ourShader, jarMatrix, glm::vec4(0.8f, 0.5f, 0.3f, 1.0f), 0.5f, 0.5f, 36);

        // Arrange kites in the jar
        // Number of kites
        int numKites = 8;

        // Loop to position kites inside the jar
        for (int i = 0; i < numKites; ++i) {
            // Spread kites in a circular pattern
            float angle = glm::radians(i * 360.0f / numKites);
            float x = 0.25f * cos(angle); // Circular placement on X-axis
            float z = 0.25f * sin(angle); // Circular placement on Z-axis
            float y = 0.1f + 0.2f * (i % 3); // Vary heights slightly within the jar

            // Kite position relative to the jar
            glm::vec3 relativePosition = glm::vec3(x, y, z);

            // Combine jarMatrix and relative position for the kite
            glm::mat4 kiteMatrix = jarMatrix * glm::translate(glm::mat4(1.0f), relativePosition);

            // Draw the kite with stick
            drawKiteWithStick(
                ourShader, kiteMatrix,
                glm::vec4(1.0f - 0.1f * i, 0.1f * i, 0.5f, 1.0f), // Kite color
                glm::vec4(0.3f, 0.2f, 0.1f, 1.0f),                // Stick color
                0.5f, glm::vec3(0.0f));                           // Position now handled by `kiteMatrix`
        }
        // Draw the river in the render loop
        
        //glBindTexture(GL_TEXTURE_2D, waterTextureID); // Replace `waterTextureID` with your texture ID

        glm::mat4 riverMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(14.0f, -0.2f, 7.0f)); // Adjust Y-position to place river under objects
        drawBezierRiver(
            ourShader,
            riverMatrix,
            riverVAO,
            riverIndices,
            glm::vec4(0.0f, 0.3f, 1.0f, 1.0f) // Blue river color
        );


        //*****************************Rides***********************************************************************
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        FerrisWheel(ourShader, translateMatrix);
        PirateShip(ourShader, translateMatrix);
        Carousal(ourShader, translateMatrix, rotateAngleTest_Y);


        //Lamp Posts
        glBindTexture(GL_TEXTURE_2D, texture0);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.0f, -0.7f));
        LampPost(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, -19.7f));
        LampPost(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(2.0f, 0.0f, -13.7f));
        LampPost(ourShader, translateMatrix);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-2.0f, 0.0f, -7.2f));
        LampPost(ourShader, translateMatrix);


       
        //balls
        //model = identityMatrix;
        //model = glm::translate(model, glm::vec3(-3.0f, -0.2f, -5.0f));
        //glm::mat4 ballmodel = glm::translate(glm::mat4(1.0f), glm::vec3(11.0f, -0.15f, 3.0f));
        
        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.0f, -0.15f, 3.3f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.4f, -0.15f, 3.3f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(10.5f, -0.15f, 2.7f));
        ball1.drawSphereWithTexture(ourShader, translateMatrix);
        
        /*if (!textureOn)
        {
            sphereWithTexture.drawSphereWithTexture(lightCubeShaderwithtexture, translateMatrix);
        }
        else
        {
            model *= glm::scale(identityMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            ball.drawSphere(ourShader);
        }
        */
        // Draw a tree at a specific location
        glm::mat4 treeModel = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -5.0f));
        drawRealisticTree(ourShader, treeModel, trunkVAO, canopyVAO, trunkIndices, canopyIndices);

        // Draw a tree at a specific location
        treeModel = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, -11.5f));
        drawRealisticTree(ourShader, treeModel, trunkVAO, canopyVAO, trunkIndices, canopyIndices);

        //********* END of Object Making **********

        //Lights
        lightCubeShader.use();
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.13f, 0.1f, 0.13f));

        for (int i = 1; i <= 5; i++)
        {

            /*glm::vec3 lightColor;
            lightColor.x = sin(glfwGetTime() * 1.0f);
            lightColor.y = sin(glfwGetTime() * 0.35f);
            lightColor.z = sin(glfwGetTime() * 0.7f);
            glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
            lightCubeShader.setVec4("bodyColor", glm::vec4(diffuseColor, 1.0f));*/

            glm::vec4 bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            //emissive
            glm::vec3 val = glm::vec3(0.5f);
            if (i == 1 and pointLightOn[0] == 0.0)
                bodyColor = glm::vec4(val, 1.0f);
            if (i == 2 and pointLightOn[1] == 0.0)
                bodyColor = glm::vec4(val, 1.0f);
            if (i == 3 and pointLightOn[2] == 0.0)
                bodyColor = glm::vec4(val, 1.0f);
            if (i == 4 and pointLightOn[3] == 0.0)
                bodyColor = glm::vec4(val, 1.0f);
            if (i == 5 and spotLightOn == 0.0)
                bodyColor = glm::vec4(val, 1.0f);


            lightCubeShader.setVec4("bodyColor", bodyColor);
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            lightCubeShader.setMat4("projection", projection);
            glm::mat4 view = camera.GetViewMatrix();
            lightCubeShader.setMat4("view", view);
            glm::mat4 tempModel = glm::mat4(1.0f);
            tempModel = glm::translate(tempModel, lightPositions[i]);
            lightCubeShader.setMat4("model", tempModel * scaleMatrix);
            lamp.drawSphere(lightCubeShader);

        }


        //Texture drawing
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();
        glm::mat4 view1 = glm::mat4(1.0f);
        glm::mat4 projection1 = glm::mat4(1.0f);
        view1 = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        projection1 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        skyboxShader.setMat4("view", view1);
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
#include "src/draw_static.h"
#include "src/bezier_utils.h"
#include "src/draw_animated.h"
