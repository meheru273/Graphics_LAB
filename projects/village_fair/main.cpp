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
unsigned int texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9,texture10,garden2,garden1,whiteflower,shoptex;

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
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
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

        translateMatrix = glm::translate(identityMatrix, glm::vec3(11.4f, -2.145f, 26.0f));
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, garden1);
        garden(ourShader, translateMatrix, color1);

        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.95f, -2.145f, 26.0f));
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, garden1);
        garden(ourShader, translateMatrix, color1);


        //Boundary
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Boundary(ourShader, translateMatrix, color1);
        translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
        color1 = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        Gate(ourShader, translateMatrix, color1);

        

       
        //Shops
        translateMatrix = glm::translate(identityMatrix, glm::vec3(6.0f, 0.0f, -2.0f));
        if (!textureOn) {
            Shop(ourShader, translateMatrix, color1, texture5);
        }
        else
        {

            Shop(ourShader, translateMatrix, color1, texture0);
        }
        
        
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::mat4 reflectionMatrix(1.0f);
        glm::vec3 normal(1.0f, 0.0f, 0.0f);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                reflectionMatrix[i][j] = (i == j) ? 1.0f - 2.0f * normal[i] * normal[j] : -2.0f * normal[i] * normal[j];
            }
        }
        translateMatrix = glm::translate(identityMatrix, glm::vec3(-7.2f, 0.0f, -15.0f));
        color1 = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        if (!textureOn) {
            Shop(ourShader, reflectionMatrix * translateMatrix, color1, texture6);
        }
        else
        {

            Shop(ourShader, reflectionMatrix * translateMatrix, color1, texture0);
        }

        translateMatrix = glm::translate(identityMatrix, glm::vec3(8.0f, 0.0f, -13.0f));
        color1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (!textureOn) {
            Shop(ourShader, translateMatrix, color1, shoptex);
        }
        else
        {

            Shop(ourShader, translateMatrix, color1, texture0);
        }
        

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

        //glass
        glass(ourShader, VAOdec2, 3.0, 0.38, 6.5, 12.0, 4.0, .025);
        glass(ourShader, VAOdec2, 2.5, 0.38, 6.9, 12.0, 4.0, .025);
        glass(ourShader, VAOdec2, 2.5, 0.38, 7.2, 12.0, 4.0, .025);
        glass(ourShader, VAOdec2, 3.0, 0.38, 6.9, 12.0, 4.0, .025);

        glass(ourShader, VAOdec2, 3.4, 0.38, 7.0, 12.0, 4.0, .025);

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
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
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
        view1 = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up)));
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
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {                 //Forward
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {                 //Backward
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {                 //Left
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {                 //Right
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)                   //Down
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)                   //Down
    {
        if (!textureOn) textureOn = true;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)                   //Pitch positive
    {
        camera.ProcessYPR(0.0f, 3.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)                   //Pitch negative
    {
        camera.ProcessYPR(0.0f, -3.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)                   //Yaw positive
    {
        camera.ProcessYPR(3.0f, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)                   //Yaw negative
    {
        camera.ProcessYPR(-3.0f, 0.0f, 0.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)                   //Roll positive
    {
        camera.ProcessYPR(0.0f, 0.0f, 0.5f);

    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)                   //Roll negative
    {
        camera.ProcessYPR(0.0f, 0.0f, -0.5f);

    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)                   //Rotate camera around a look at point
    {                                                                   //Right
        camera.RotateAroundLookAt(2.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)                   //Rotate camera around a look at point
    {                                                                   //Left
        camera.RotateAroundLookAt(-2.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)                   //Up
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.changeEye(eyeX, eyeY, eyeZ);

        camera.ProcessKeyboard(UP, deltaTime);

    }


    //if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)               //Basic Camera rotate around lookat
    //{
    //    //lookAtX += 2.5 * deltaTime;
    //    //basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    //    float change = -0.3f;
    //    float x, y, z;
    //    x = eyeX * cos(glm::radians(change)) + eyeZ * sin(glm::radians(change));
    //    y = eyeY;
    //    z = - eyeX * sin(glm::radians(change)) + eyeZ * cos(glm::radians(change));

    //    eyeX = x, eyeY = y, eyeZ = z;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    //{
    //    //lookAtX -= 2.5 * deltaTime;
    //    //basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);

    //    float change = 0.3f;
    //    float x, y, z;
    //    x = eyeX * cos(glm::radians(change)) + eyeZ * sin(glm::radians(change));
    //    y = eyeY;
    //    z = -eyeX * sin(glm::radians(change)) + eyeZ * cos(glm::radians(change));

    //    eyeX = x, eyeY = y, eyeZ = z;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        lookAtZ += 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        lookAtZ -= 2.5 * deltaTime;
        basic_camera.changeLookAt(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        basic_camera.changeViewUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
    }

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_G && action == GLFW_PRESS)                   //Gate Open/Close
    {
        isGateOpening *= -1.0f;
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


//Texture Loading

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

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


void read_file(string file_name, vector<float> &vec)
{
    ifstream file(file_name);
    float number;

    while (file >> number)
        vec.push_back(number);

    file.close();
}
long long nCr(int n, int r)
{
    if (r > n / 2)
        r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for (i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve(double t, float xy[2], GLfloat ctrlpoints[], int L)
{
    double y = 0;
    double x = 0;
    t = t > 1.0 ? 1.0 : t;
    for (int i = 0; i < L + 1; i++)
    {
        long long ncr = nCr(L, i);
        double oneMinusTpow = pow(1 - t, double(L - i));
        double tPow = pow(t, double(i));
        double coef = oneMinusTpow * tPow * ncr;
        x += coef * ctrlpoints[i * 3];
        y += coef * ctrlpoints[(i * 3) + 1];

    }
    xy[0] = float(x);
    xy[1] = float(y);
}

unsigned int hollowBezier(GLfloat ctrlpoints[], int L, vector<float>& coordinates, vector<float>& normals, vector<int>& indices, vector<float>& vertices)
{
    int i, j;
    float x, y, z, r;                //current coordinates
    float theta;
    float nx, ny, nz, lengthInv;    // vertex normal


    const float dtheta = 2 * pi / ntheta;        //angular step size

    float t = 0;
    float dt = 1.0 / nt;
    float xy[2];

    for (i = 0; i <= nt; ++i)              //step through y
    {
        BezierCurve(t, xy, ctrlpoints, L);
        r = xy[0];
        y = xy[1];
        theta = 0;
        t += dt;
        lengthInv = 1.0 / r;

        for (j = 0; j <= ntheta; ++j)
        {
            double cosa = cos(theta);
            double sina = sin(theta);
            z = r * cosa;
            x = r * sina;

            coordinates.push_back(x);
            coordinates.push_back(y);
            coordinates.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            // center point of the circle (0,y,0)
            nx = (x - 0) * lengthInv;
            ny = (y - y) * lengthInv;
            nz = (z - 0) * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            theta += dtheta;
        }
    }

    // generate index list of triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1

    int k1, k2;
    for (int i = 0; i < nt; ++i)
    {
        k1 = i * (ntheta + 1);     // beginning of current stack
        k2 = k1 + ntheta + 1;      // beginning of next stack

        for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
        {
            // k1 => k2 => k1+1
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // k1+1 => k2 => k2+1
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    size_t count = coordinates.size();
    for (int i = 0; i < count; i += 3)
    {
        vertices.push_back(coordinates[i]);
        vertices.push_back(coordinates[i + 1]);
        vertices.push_back(coordinates[i + 2]);

        vertices.push_back(normals[i]);
        vertices.push_back(normals[i + 1]);
        vertices.push_back(normals[i + 2]);
    }

    unsigned int bezierVAO;
    glGenVertexArrays(1, &bezierVAO);
    glBindVertexArray(bezierVAO);

    // create VBO to copy vertex data to VBO
    unsigned int bezierVBO;
    glGenBuffers(1, &bezierVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
    glBufferData(GL_ARRAY_BUFFER,                   // target
        (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
        vertices.data(),   // ptr to vertex data
        GL_STATIC_DRAW);                   // usage

    // create EBO to copy index data
    unsigned int bezierEBO;
    glGenBuffers(1, &bezierEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
        (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
        indices.data(),               // ptr to index data
        GL_STATIC_DRAW);                   // usage

    // activate attrib arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // set attrib arrays with stride and offset
    int stride = 24;     // should be 24 bytes
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));

    // unbind VAO, VBO and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return bezierVAO;
}

void drawCurve(Shader ourShader, glm::mat4 moveMatrix, unsigned int VAO, vector<int> indices, glm::vec4 color, float rotateAngleTest_Y)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateYMatrix;

    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Y), glm::vec3(0.0f, 1.0f, 0.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateYMatrix * scaleMatrix;

    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 32.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void FerrisWheel(Shader ourShader, glm::mat4 moveMatrix)
{

    //front
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    if (isFerrisWheelOn)
    {
        ferrisWheelSpeed += ferrisIncrement;
        ferrisWheelSpeed = min(2.5f, ferrisWheelSpeed);
    }
    else
    {
        ferrisWheelSpeed -= ferrisIncrement;
        ferrisWheelSpeed = max(0.02f, ferrisWheelSpeed);
    }

    if (ferrisWheelSpeed != 0.02f)
        ferrisWheelAngle += ferrisWheelSpeed;

    ferrisWheelAngle = fmod(ferrisWheelAngle, 360);
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 12.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;

    glm::vec4 color = glm::vec4(100.0 / 256, 100.0 / 256, 200.0 / 256, 1.0f);
    //glm::vec4 color = glm::vec4(1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 32.0f);
    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //bars
    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(0.0f), 1.0f);

    pivot = translateMatrix * pivot;

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 11.975f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 0.1f));
    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }



    //back
    rotateTemp = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(ferrisWheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 11.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
    model = translateMatrix * rotateZMatrix * scaleMatrix * rotateTemp;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(bezierVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //bars

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 2.0f, 10.975f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 3.0f, 0.1f));
    for (float i = 0.0f; i >= -360.0f; i -= 45.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * rotateTemp * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    //connecting bars
    //middle
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.975f, 1.975f, 10.5f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 4.0f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //top ones
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.0f, 3.5f, 10.995f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 2.0f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    for (float i = 0.0f; i >= -360.0f; i -= 90.0f)
    {
        rotateTemp = glm::rotate(identityMatrix, glm::radians(i), glm::vec3(0.0f, 0.0f, 1.0f));
        model = translateFromPivot * rotateZMatrix * translateToPivot * translateFromPivot * rotateTemp * translateToPivot * translateMatrix * scaleMatrix;
        ourShader.setMat4("model", moveMatrix * model);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //pillars
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, 12.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, 12.1f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(42.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.9f, -0.41f, 10.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(5.1f, -0.47f, 10.75f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.2f, 6.6f, 0.2f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(42.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color * 0.5f);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    color = glm::vec4(1.0f);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    //sitting places
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(1.5f, 1.5f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(0.0f, 3.0f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(-1.5f, 1.5f, 12.0f));
    FerrisWheelSeat(ourShader, translateMatrix, ferrisWheelAngle);


}

void FerrisWheelSeat(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z)
{
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrixLocal, rotateZMatrixMain, rotateTemp;
    glm::mat4 translateToPivotMain, translateFromPivotMain;
    glm::vec4 translateToPivotOrgMain, translateFromPivotOrgMain;
    glm::mat4 translateToPivotLocal, translateFromPivotLocal;
    glm::vec4 translateToPivotOrgLocal, translateFromPivotOrgLocal;
    glm::vec4 pivot1, pivot2;
    glm::vec4 mainPivotPoint = glm::vec4(3.0f, 2.0f, 0.0f, 1.0);
    glm::vec4 localPivotPoint = glm::vec4(2.975f, 0.475f, -1.005f, 1.0);

    pivot1 = mainPivotPoint;
    translateToPivotMain = glm::translate(identityMatrix, glm::vec3(-pivot1));
    translateFromPivotMain = glm::translate(identityMatrix, glm::vec3(pivot1));
    rotateZMatrixMain = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    pivot2 = translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * moveMatrix * localPivotPoint;
    translateToPivotLocal = glm::translate(identityMatrix, glm::vec3(-pivot2));
    translateFromPivotLocal = glm::translate(identityMatrix, glm::vec3(pivot2));
    rotateZMatrixLocal = glm::rotate(identityMatrix, glm::radians(-rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));

    glBindTexture(GL_TEXTURE_2D, texture3);
    //base
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(2.0f, 0.1f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, texture10);
    //left
    //seat
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.5f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //seat right
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.2f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.6f, 0.5f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //back
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.45f, -0.2f, -0.9f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 1.0f, 1.5f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //top bars
    glBindTexture(GL_TEXTURE_2D, texture0);
    //left
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.899f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.15f, 0.1f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.5f, 0.25f, -0.1751f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(1.15f, 0.1f, 0.05f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //right
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.8f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.3f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.1f, 0.1f, 0.1f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.899f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.15f, 0.1f, 0.05f));
    rotateTemp = glm::rotate(identityMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //4
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.5f, 0.25f, -0.1751f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(-1.15f, 0.1f, 0.05f));
    model = translateFromPivotLocal * rotateZMatrixLocal * translateToPivotLocal * translateFromPivotMain * rotateZMatrixMain * translateToPivotMain * translateMatrix * rotateTemp * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


}
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

void PirateShip(Shader ourShader, glm::mat4 moveMatrix, float rotateAngleTest_Z)
{

    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, scaleMatrix, model, rotateZMatrix, rotateTemp;

    glm::mat4 translateToPivot, translateFromPivot;
    glm::vec4 translateToPivotOrg, translateFromPivotOrg;
    glm::vec4 pivot = glm::vec4(glm::vec3(14.0f, 2.0f, 4.2f), 1.0f);

    translateToPivot = glm::translate(identityMatrix, glm::vec3(-pivot));
    translateFromPivot = glm::translate(identityMatrix, glm::vec3(pivot));

    glm::vec4 color = glm::vec4(1.0f, 10.0 / 256, 20.0 / 256, 1.0f);

    float currentTime = static_cast<float>(glfwGetTime());
    if (currentTime - lastTime > 0.1)
    {
        if (isBoatOn)
            boatAngle += boatSpeed;
        if (abs(boatAngle) >= boatMaxAngle)
        {
            boatSpeed *= -1;
            lastTime = static_cast<float>(glfwGetTime());
        }
    }
    if (!isBoatOn)
    {
        if (boatAngle < 0)
        {
            boatAngle += abs(boatSpeed);
        }
        if (boatAngle > 0)
        {
            boatAngle -= abs(boatSpeed);
        }
    }


    rotateAngleTest_Z = boatAngle;
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngleTest_Z), glm::vec3(0.0f, 0.0f, 1.0f));
    //dynamic
    //boat
    glBindTexture(GL_TEXTURE_2D, texture3);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, -1.18f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.7f, 0.7f, 0.3f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
    //base
    color = glm::vec4(150.0 / 256, 12.0 / 256, 35.0 / 256, 1.0f);
    translateMatrix = glm::translate(identityMatrix, glm::vec3(14.0f, -0.45f, 7.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.55f, 0.2f, 0.248f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color * 0.5f);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);
    glBindVertexArray(boatVAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indicesBoat.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, texture0);
    //bars
    color = glm::vec4(1.0f);
    ourShader.setMat4("model", moveMatrix * model);
    ourShader.setVec4("material.ambient", color);
    ourShader.setVec4("material.diffuse", color);
    ourShader.setVec4("material.specular", color);
    ourShader.setFloat("material.shininess", 256.0f);

    float moveX, moveY, moveZ;
    moveX = 11.0f, moveY = -0.4f, moveZ = 7.7f;
    //1
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.975f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //2
    translateMatrix = glm::translate(identityMatrix, glm::vec3(2.575f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    //3
    translateMatrix = glm::translate(identityMatrix, glm::vec3(3.375f + moveX, 0.5f + moveY, -1.2f + moveZ));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(0.1f, 0.1f, 1.95f));
    model = translateFromPivot * rotateZMatrix * translateToPivot * translateMatrix * scaleMatrix;
    ourShader.setMat4("model", moveMatrix * model);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    
}

// Function to draw a tree using B�zier curves
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
