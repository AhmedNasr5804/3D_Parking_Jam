#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Screen dimensions
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.6f, -0.8f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Game state
enum GameState { MENU, LEVEL_SELECT, PLAYING, PAUSED, GAME_OVER, WIN };
GameState gameState = MENU;
int score = 1000;
float gameTime = 120.0f;
int moveCount = 0;
int selectedCarIndex = -1;
int currentLevel = 1;
int totalLevels = 3;

// Mouse state
double mouseX, mouseY;
bool mouseClicked = false;

// Keys pressed tracking
bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;

// Collision penalty tracker
float collisionCooldown = 0.0f;

// Menu button structure
struct Button {
    float x, y, width, height;
    std::string text;
    bool hovered;
    int id;
};

std::vector<Button> menuButtons;
std::vector<Button> pauseButtons;
std::vector<Button> levelButtons;

// Complete bitmap font data with all letters and numbers
std::map<char, std::vector<std::vector<int>>> fontData = {
    {'0', {{0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'1', {{0,0,1,0,0}, {0,1,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,1,1,1,0}}},
    {'2', {{0,1,1,1,0}, {1,0,0,0,1}, {0,0,0,0,1}, {0,0,1,1,0}, {0,1,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}}},
    {'3', {{1,1,1,1,0}, {0,0,0,0,1}, {0,0,1,1,0}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'4', {{0,0,0,1,0}, {0,0,1,1,0}, {0,1,0,1,0}, {1,0,0,1,0}, {1,1,1,1,1}, {0,0,0,1,0}, {0,0,0,1,0}}},
    {'5', {{1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,0}, {0,0,0,0,1}, {0,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'6', {{0,1,1,1,0}, {1,0,0,0,0}, {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'7', {{1,1,1,1,1}, {0,0,0,0,1}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}}},
    {'8', {{0,1,1,1,0}, {1,0,0,0,1}, {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'9', {{0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,1}, {0,0,0,0,1}, {0,0,0,0,1}, {0,1,1,1,0}}},
    {':', {{0,0,0,0,0}, {0,1,1,0,0}, {0,1,1,0,0}, {0,0,0,0,0}, {0,1,1,0,0}, {0,1,1,0,0}, {0,0,0,0,0}}},
    {' ', {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}}},
    {'A', {{0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}}},
    {'B', {{1,1,1,1,0}, {1,0,0,0,1}, {1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0}}},
    {'C', {{0,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {0,1,1,1,1}}},
    {'D', {{1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0}}},
    {'E', {{1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}}},
    {'F', {{1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}}},
    {'G', {{0,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'H', {{1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}}},
    {'I', {{1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {1,1,1,1,1}}},
    {'K', {{1,0,0,0,1}, {1,0,0,1,0}, {1,1,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}}},
    {'L', {{1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}}},
    {'M', {{1,0,0,0,1}, {1,1,0,1,1}, {1,0,1,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}}},
    {'N', {{1,0,0,0,1}, {1,1,0,0,1}, {1,0,1,0,1}, {1,0,0,1,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}}},
    {'O', {{0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'P', {{1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}}},
    {'R', {{1,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,1,1,1,0}, {1,0,1,0,0}, {1,0,0,1,0}, {1,0,0,0,1}}},
    {'S', {{0,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}, {0,1,1,1,0}, {0,0,0,0,1}, {0,0,0,0,1}, {1,1,1,1,0}}},
    {'T', {{1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}}},
    {'U', {{1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}}},
    {'V', {{1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0}}},
    {'W', {{1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,1,0,1}, {1,0,1,0,1}, {1,1,0,1,1}, {1,0,0,0,1}}},
    {'Y', {{1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}}},
};

// Vertex shader source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

const char* vertex2DShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    ourColor = aColor;
}
)";

const char* fragment2DShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
     0.5f, -0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
     0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
     0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
    -0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
    -0.5f, -0.5f, -0.5f,  0.8f, 0.2f, 0.2f,

    -0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
     0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
     0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
     0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
    -0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
    -0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.3f,

    -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.2f,
    -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.2f,
    -0.5f, -0.5f, -0.5f,  0.7f, 0.2f, 0.2f,
    -0.5f, -0.5f, -0.5f,  0.7f, 0.2f, 0.2f,
    -0.5f, -0.5f,  0.5f,  0.7f, 0.2f, 0.2f,
    -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.2f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.4f, 0.4f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.4f, 0.4f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.4f, 0.4f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.4f, 0.4f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.4f, 0.4f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.4f, 0.4f,

    -0.5f, -0.5f, -0.5f,  0.6f, 0.1f, 0.1f,
     0.5f, -0.5f, -0.5f,  0.6f, 0.1f, 0.1f,
     0.5f, -0.5f,  0.5f,  0.6f, 0.1f, 0.1f,
     0.5f, -0.5f,  0.5f,  0.6f, 0.1f, 0.1f,
    -0.5f, -0.5f,  0.5f,  0.6f, 0.1f, 0.1f,
    -0.5f, -0.5f, -0.5f,  0.6f, 0.1f, 0.1f,

    -0.5f,  0.5f, -0.5f,  0.9f, 0.3f, 0.3f,
     0.5f,  0.5f, -0.5f,  0.9f, 0.3f, 0.3f,
     0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
     0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
    -0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.3f,
    -0.5f,  0.5f, -0.5f,  0.9f, 0.3f, 0.3f
};

struct Car {
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 baseColor;
    bool isVertical;
    bool isTarget;
    int id;
    float minPos;
    float maxPos;
};

std::vector<Car> cars;

void setupLevel1() {
    cars.clear();
    gameTime = 120.0f;
    score = 1000;
    moveCount = 0;
    
    cars.push_back({glm::vec3(-3.0f, 0.4f, 0.0f), glm::vec3(2.5f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 0.0f, 0.0f), false, true, 0, -5.0f, 6.5f});
    cars.push_back({glm::vec3(1.0f, 0.4f, 2.5f), glm::vec3(1.2f, 0.8f, 2.5f), 
                    glm::vec3(0.2f, 0.5f, 1.0f), true, false, 1, -4.0f, 4.0f});
    cars.push_back({glm::vec3(3.0f, 0.4f, -1.0f), glm::vec3(1.2f, 0.8f, 3.0f), 
                    glm::vec3(0.2f, 1.0f, 0.5f), true, false, 2, -4.0f, 4.0f});
    cars.push_back({glm::vec3(-1.5f, 0.4f, -3.0f), glm::vec3(2.0f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 1.0f, 0.2f), false, false, 3, -5.0f, 5.0f});
}

void setupLevel2() {
    cars.clear();
    gameTime = 150.0f;
    score = 1500;
    moveCount = 0;
    
    cars.push_back({glm::vec3(-4.0f, 0.4f, 0.0f), glm::vec3(2.5f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 0.0f, 0.0f), false, true, 0, -5.0f, 6.5f});
    cars.push_back({glm::vec3(0.0f, 0.4f, 2.0f), glm::vec3(1.2f, 0.8f, 2.5f), 
                    glm::vec3(0.2f, 0.5f, 1.0f), true, false, 1, -4.0f, 4.0f});
    cars.push_back({glm::vec3(2.5f, 0.4f, 0.0f), glm::vec3(1.2f, 0.8f, 3.0f), 
                    glm::vec3(0.2f, 1.0f, 0.5f), true, false, 2, -4.0f, 4.0f});
    cars.push_back({glm::vec3(-2.0f, 0.4f, -2.5f), glm::vec3(2.0f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 1.0f, 0.2f), false, false, 3, -5.0f, 5.0f});
    cars.push_back({glm::vec3(-3.5f, 0.4f, 3.0f), glm::vec3(1.2f, 0.8f, 2.0f), 
                    glm::vec3(1.0f, 0.5f, 0.0f), true, false, 4, -4.0f, 4.0f});
    cars.push_back({glm::vec3(4.0f, 0.4f, -3.0f), glm::vec3(1.2f, 0.8f, 2.0f), 
                    glm::vec3(0.5f, 0.0f, 1.0f), true, false, 5, -4.0f, 4.0f});
}

void setupLevel3() {
    cars.clear();
    gameTime = 180.0f;
    score = 2000;
    moveCount = 0;
    
    cars.push_back({glm::vec3(-4.5f, 0.4f, 0.0f), glm::vec3(2.5f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 0.0f, 0.0f), false, true, 0, -5.0f, 6.5f});
    cars.push_back({glm::vec3(-1.0f, 0.4f, 2.0f), glm::vec3(1.2f, 0.8f, 2.5f), 
                    glm::vec3(0.2f, 0.5f, 1.0f), true, false, 1, -4.0f, 4.0f});
    cars.push_back({glm::vec3(1.5f, 0.4f, 0.5f), glm::vec3(1.2f, 0.8f, 3.0f), 
                    glm::vec3(0.2f, 1.0f, 0.5f), true, false, 2, -4.0f, 4.0f});
    cars.push_back({glm::vec3(-2.5f, 0.4f, -2.5f), glm::vec3(2.0f, 0.8f, 1.2f), 
                    glm::vec3(1.0f, 1.0f, 0.2f), false, false, 3, -5.0f, 5.0f});
    cars.push_back({glm::vec3(-3.5f, 0.4f, 3.5f), glm::vec3(1.2f, 0.8f, 2.0f), 
                    glm::vec3(1.0f, 0.5f, 0.0f), true, false, 4, -4.0f, 4.0f});
    cars.push_back({glm::vec3(4.0f, 0.4f, -2.0f), glm::vec3(1.2f, 0.8f, 2.5f), 
                    glm::vec3(0.5f, 0.0f, 1.0f), true, false, 5, -4.0f, 4.0f});
    cars.push_back({glm::vec3(1.0f, 0.4f, -4.0f), glm::vec3(2.5f, 0.8f, 1.2f), 
                    glm::vec3(0.0f, 0.8f, 0.8f), false, false, 6, -5.0f, 5.0f});
}

void loadLevel(int level) {
    currentLevel = level;
    switch(level) {
        case 1: setupLevel1(); break;
        case 2: setupLevel2(); break;
        case 3: setupLevel3(); break;
        default: setupLevel1(); break;
    }
    selectedCarIndex = 0;
}

void setupMenuButtons() {
    menuButtons.clear();
    menuButtons.push_back({400, 300, 400, 80, "START GAME", false, 0});
    menuButtons.push_back({400, 420, 400, 80, "EXIT", false, 1});
}

void setupPauseButtons() {
    pauseButtons.clear();
    pauseButtons.push_back({400, 300, 400, 80, "RESUME", false, 0});
    pauseButtons.push_back({400, 420, 400, 80, "MAIN MENU", false, 1});
}

void setupLevelButtons() {
    levelButtons.clear();
    levelButtons.push_back({300, 250, 200, 100, "LEVEL 1", false, 1});
    levelButtons.push_back({550, 250, 200, 100, "LEVEL 2", false, 2});
    levelButtons.push_back({800, 250, 200, 100, "LEVEL 3", false, 3});
    levelButtons.push_back({400, 450, 400, 80, "BACK", false, 0});
}

bool checkCollision(int carIndex, glm::vec3 newPos) {
    Car& car = cars[carIndex];
    
    for (size_t i = 0; i < cars.size(); i++) {
        if (i == carIndex) continue;
        
        const Car& other = cars[i];
        
        bool collisionX = (newPos.x - car.size.x/2 < other.position.x + other.size.x/2) &&
                         (newPos.x + car.size.x/2 > other.position.x - other.size.x/2);
        
        bool collisionZ = (newPos.z - car.size.z/2 < other.position.z + other.size.z/2) &&
                         (newPos.z + car.size.z/2 > other.position.z - other.size.z/2);
        
        if (collisionX && collisionZ) {
            return true;
        }
    }
    
    return false;
}

void drawCube(unsigned int shaderProgram, unsigned int VAO, glm::mat4 model) {
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawParkingLot(unsigned int shaderProgram, unsigned int VAO, unsigned int colorVBO) {
    float groundColor[] = {0.3f, 0.3f, 0.35f};
    
    std::vector<float> groundVertices;
    for (int i = 0; i < 36; i++) {
        groundVertices.push_back(cubeVertices[i * 6]);
        groundVertices.push_back(cubeVertices[i * 6 + 1]);
        groundVertices.push_back(cubeVertices[i * 6 + 2]);
        groundVertices.push_back(groundColor[0]);
        groundVertices.push_back(groundColor[1]);
        groundVertices.push_back(groundColor[2]);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, groundVertices.size() * sizeof(float), groundVertices.data());
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(12.0f, 0.1f, 12.0f));
    drawCube(shaderProgram, VAO, model);
    
    float lineColor[] = {0.9f, 0.9f, 0.9f};
    std::vector<float> lineVertices;
    for (int i = 0; i < 36; i++) {
        lineVertices.push_back(cubeVertices[i * 6]);
        lineVertices.push_back(cubeVertices[i * 6 + 1]);
        lineVertices.push_back(cubeVertices[i * 6 + 2]);
        lineVertices.push_back(lineColor[0]);
        lineVertices.push_back(lineColor[1]);
        lineVertices.push_back(lineColor[2]);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(float), lineVertices.data());
    
    for (int i = -5; i <= 5; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.06f, i * 1.2f));
        model = glm::scale(model, glm::vec3(11.0f, 0.01f, 0.05f));
        drawCube(shaderProgram, VAO, model);
    }
    
    for (int i = -5; i <= 5; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i * 1.2f, 0.06f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.01f, 11.0f));
        drawCube(shaderProgram, VAO, model);
    }
    
    float exitColor[] = {0.2f, 0.8f, 0.2f};
    std::vector<float> exitVertices;
    for (int i = 0; i < 36; i++) {
        exitVertices.push_back(cubeVertices[i * 6]);
        exitVertices.push_back(cubeVertices[i * 6 + 1]);
        exitVertices.push_back(cubeVertices[i * 6 + 2]);
        exitVertices.push_back(exitColor[0]);
        exitVertices.push_back(exitColor[1]);
        exitVertices.push_back(exitColor[2]);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, exitVertices.size() * sizeof(float), exitVertices.data());
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.8f, 0.06f, 0.0f));
    model = glm::scale(model, glm::vec3(0.3f, 0.02f, 2.0f));
    drawCube(shaderProgram, VAO, model);
}

void drawCar(unsigned int shaderProgram, unsigned int VAO, unsigned int colorVBO, const Car& car, bool isSelected) {
    glm::vec3 color = car.baseColor;
    
    if (isSelected) {
        color = glm::vec3(
            glm::min(car.baseColor.r + 0.3f, 1.0f),
            glm::min(car.baseColor.g + 0.3f, 1.0f),
            glm::min(car.baseColor.b + 0.3f, 1.0f)
        );
    }
    
    std::vector<float> carVertices;
    for (int i = 0; i < 36; i++) {
        carVertices.push_back(cubeVertices[i * 6]);
        carVertices.push_back(cubeVertices[i * 6 + 1]);
        carVertices.push_back(cubeVertices[i * 6 + 2]);
        carVertices.push_back(color.r);
        carVertices.push_back(color.g);
        carVertices.push_back(color.b);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, carVertices.size() * sizeof(float), carVertices.data());
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, car.position);
    model = glm::scale(model, car.size);
    drawCube(shaderProgram, VAO, model);
    
    float wheelColor[] = {0.1f, 0.1f, 0.1f};
    std::vector<float> wheelVertices;
    for (int i = 0; i < 36; i++) {
        wheelVertices.push_back(cubeVertices[i * 6]);
        wheelVertices.push_back(cubeVertices[i * 6 + 1]);
        wheelVertices.push_back(cubeVertices[i * 6 + 2]);
        wheelVertices.push_back(wheelColor[0]);
        wheelVertices.push_back(wheelColor[1]);
        wheelVertices.push_back(wheelColor[2]);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, wheelVertices.size() * sizeof(float), wheelVertices.data());
    
    float wheelSize = 0.25f;
    float wheelOffset = car.isVertical ? car.size.z * 0.35f : car.size.x * 0.35f;
    float wheelHeight = -car.size.y * 0.35f;
    
    glm::vec3 wheelPositions[4];
    if (car.isVertical) {
        wheelPositions[0] = car.position + glm::vec3(-car.size.x * 0.35f, wheelHeight, wheelOffset);
        wheelPositions[1] = car.position + glm::vec3(car.size.x * 0.35f, wheelHeight, wheelOffset);
        wheelPositions[2] = car.position + glm::vec3(-car.size.x * 0.35f, wheelHeight, -wheelOffset);
        wheelPositions[3] = car.position + glm::vec3(car.size.x * 0.35f, wheelHeight, -wheelOffset);
    } else {
        wheelPositions[0] = car.position + glm::vec3(wheelOffset, wheelHeight, -car.size.z * 0.35f);
        wheelPositions[1] = car.position + glm::vec3(wheelOffset, wheelHeight, car.size.z * 0.35f);
        wheelPositions[2] = car.position + glm::vec3(-wheelOffset, wheelHeight, -car.size.z * 0.35f);
        wheelPositions[3] = car.position + glm::vec3(-wheelOffset, wheelHeight, car.size.z * 0.35f);
    }
    
    for (int i = 0; i < 4; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, wheelPositions[i]);
        model = glm::scale(model, glm::vec3(wheelSize, wheelSize, wheelSize));
        drawCube(shaderProgram, VAO, model);
    }
}

void drawRect(unsigned int shader2D, unsigned int VAO2D, unsigned int VBO2D, 
              float x, float y, float w, float h, glm::vec3 color) {
    float vertices[] = {
        x, y, color.r, color.g, color.b,
        x + w, y, color.r, color.g, color.b,
        x + w, y + h, color.r, color.g, color.b,
        x + w, y + h, color.r, color.g, color.b,
        x, y + h, color.r, color.g, color.b,
        x, y, color.r, color.g, color.b
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO2D);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    glUseProgram(shader2D);
    glBindVertexArray(VAO2D);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawChar(unsigned int shader2D, unsigned int VAO2D, unsigned int VBO2D, 
              char c, float x, float y, float scale, glm::vec3 color) {
    c = toupper(c); // Convert to uppercase for matching
    if (fontData.find(c) == fontData.end()) return;
    
    auto& pattern = fontData[c];
    float pixelSize = scale;
    
    for (int row = 0; row < pattern.size(); row++) {
        for (int col = 0; col < pattern[row].size(); col++) {
            if (pattern[row][col] == 1) {
                drawRect(shader2D, VAO2D, VBO2D,
                        x + col * pixelSize, y + row * pixelSize,
                        pixelSize, pixelSize, color);
            }
        }
    }
}

void drawText(unsigned int shader2D, unsigned int VAO2D, unsigned int VBO2D,
              const std::string& text, float x, float y, float scale, glm::vec3 color) {
    float currentX = x;
    for (char c : text) {
        drawChar(shader2D, VAO2D, VBO2D, c, currentX, y, scale, color);
        currentX += 6 * scale;
    }
}

void drawButton(unsigned int shader2D, unsigned int VAO2D, unsigned int VBO2D,
                const Button& btn) {
    glm::vec3 bgColor = btn.hovered ? glm::vec3(0.4f, 0.6f, 0.8f) : glm::vec3(0.2f, 0.3f, 0.5f);
    glm::vec3 borderColor = glm::vec3(0.8f, 0.8f, 0.8f);
    
    drawRect(shader2D, VAO2D, VBO2D, btn.x, btn.y, btn.width, btn.height, bgColor);
    
    float borderWidth = 3.0f;
    drawRect(shader2D, VAO2D, VBO2D, btn.x, btn.y, btn.width, borderWidth, borderColor);
    drawRect(shader2D, VAO2D, VBO2D, btn.x, btn.y + btn.height - borderWidth, btn.width, borderWidth, borderColor);
    drawRect(shader2D, VAO2D, VBO2D, btn.x, btn.y, borderWidth, btn.height, borderColor);
    drawRect(shader2D, VAO2D, VBO2D, btn.x + btn.width - borderWidth, btn.y, borderWidth, btn.height, borderColor);
    
    float textWidth = btn.text.length() * 6 * 3;
    float textX = btn.x + (btn.width - textWidth) / 2;
    float textY = btn.y + (btn.height - 21) / 2;
    drawText(shader2D, VAO2D, VBO2D, btn.text, textX, textY, 3.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void updateButtonHover(std::vector<Button>& buttons, double mx, double my) {
    for (auto& btn : buttons) {
        btn.hovered = (mx >= btn.x && mx <= btn.x + btn.width &&
                      my >= btn.y && my <= btn.y + btn.height);
    }
}

int checkButtonClick(const std::vector<Button>& buttons, double mx, double my) {
    for (size_t i = 0; i < buttons.size(); i++) {
        if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
            my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
            return buttons[i].id;
        }
    }
    return -1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseClicked = true;
        
        if (gameState == MENU) {
            int clicked = checkButtonClick(menuButtons, mouseX, mouseY);
            if (clicked == 0) {
                gameState = LEVEL_SELECT;
                std::cout << "Select a level!" << std::endl;
            } else if (clicked == 1) {
                glfwSetWindowShouldClose(window, true);
            }
        } else if (gameState == LEVEL_SELECT) {
            int clicked = checkButtonClick(levelButtons, mouseX, mouseY);
            if (clicked == 0) {
                gameState = MENU;
            } else if (clicked >= 1 && clicked <= 3) {
                loadLevel(clicked);
                gameState = PLAYING;
                std::cout << "Starting Level " << clicked << std::endl;
            }
        } else if (gameState == PAUSED) {
            int clicked = checkButtonClick(pauseButtons, mouseX, mouseY);
            if (clicked == 0) {
                gameState = PLAYING;
                std::cout << "Game Resumed!" << std::endl;
            } else if (clicked == 1) {
                gameState = MENU;
                std::cout << "Back to Main Menu" << std::endl;
            }
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
    
    if (gameState == MENU) {
        updateButtonHover(menuButtons, mouseX, mouseY);
    } else if (gameState == LEVEL_SELECT) {
        updateButtonHover(levelButtons, mouseX, mouseY);
    } else if (gameState == PAUSED) {
        updateButtonHover(pauseButtons, mouseX, mouseY);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_UP) upPressed = true;
        if (key == GLFW_KEY_DOWN) downPressed = true;
        if (key == GLFW_KEY_LEFT) leftPressed = true;
        if (key == GLFW_KEY_RIGHT) rightPressed = true;
        
        if (key == GLFW_KEY_P && gameState == PLAYING) {
            gameState = PAUSED;
            std::cout << "Game Paused" << std::endl;
        } else if (key == GLFW_KEY_P && gameState == PAUSED) {
            gameState = PLAYING;
            std::cout << "Game Resumed" << std::endl;
        }
        
        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
            int carNum = key - GLFW_KEY_1;
            if (carNum < cars.size() && gameState == PLAYING) {
                selectedCarIndex = carNum;
                std::cout << "Selected Car " << (carNum + 1) << std::endl;
            }
        }
        
        if (key == GLFW_KEY_SPACE && gameState == PLAYING) {
            selectedCarIndex = 0;
            std::cout << "Selected Target Car (RED)" << std::endl;
        }
        
        if (key == GLFW_KEY_R && (gameState == WIN || gameState == GAME_OVER)) {
            loadLevel(currentLevel);
            gameState = PLAYING;
            std::cout << "Level Restarted!" << std::endl;
        }
    }
    
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_UP) upPressed = false;
        if (key == GLFW_KEY_DOWN) downPressed = false;
        if (key == GLFW_KEY_LEFT) leftPressed = false;
        if (key == GLFW_KEY_RIGHT) rightPressed = false;
    }
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (gameState == PLAYING) {
            gameState = PAUSED;
        } else if (gameState == PAUSED) {
            gameState = PLAYING;
        } else if (gameState == LEVEL_SELECT) {
            gameState = MENU;
        } else {
            glfwSetWindowShouldClose(window, true);
        }
    }
    
    if (gameState != PLAYING) return;
    if (selectedCarIndex < 0 || selectedCarIndex >= cars.size()) return;
    
    // Update collision cooldown
    if (collisionCooldown > 0.0f) {
        collisionCooldown -= deltaTime;
    }
    
    Car& car = cars[selectedCarIndex];
    float moveSpeed = 3.0f * deltaTime;
    glm::vec3 newPos = car.position;
    bool moved = false;
    
    if (car.isVertical) {
        if (upPressed) {
            newPos.z -= moveSpeed;
            moved = true;
        }
        if (downPressed) {
            newPos.z += moveSpeed;
            moved = true;
        }
        newPos.z = glm::clamp(newPos.z, car.minPos, car.maxPos);
    } else {
        if (leftPressed) {
            newPos.x -= moveSpeed;
            moved = true;
        }
        if (rightPressed) {
            newPos.x += moveSpeed;
            moved = true;
        }
        newPos.x = glm::clamp(newPos.x, car.minPos, car.maxPos);
    }
    
    if (moved) {
        if (checkCollision(selectedCarIndex, newPos)) {
            // Collision detected - apply penalty
            if (collisionCooldown <= 0.0f) {
                score -= 10;
                if (score < 0) score = 0;
                collisionCooldown = 0.5f; // Penalty cooldown
                std::cout << "Collision! -10 points. Score: " << score << std::endl;
            }
        } else {
            // No collision - move the car
            car.position = newPos;
            
            // Check win condition for target car
            if (car.isTarget && car.position.x >= 5.5f) {
                gameState = WIN;
                score += (int)(gameTime * 10);
                std::cout << "LEVEL " << currentLevel << " COMPLETE! Final Score: " << score << std::endl;
            }
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Parking Jam - Enhanced Edition", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create Window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load GLAD!\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    unsigned int shader2D = createShaderProgram(vertex2DShaderSource, fragment2DShaderSource);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VBO2D, VAO2D;
    glGenVertexArrays(1, &VAO2D);
    glGenBuffers(1, &VBO2D);

    glBindVertexArray(VAO2D);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2D);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    setupMenuButtons();
    setupPauseButtons();
    setupLevelButtons();
    
    std::cout << "=== 3D PARKING JAM - ENHANCED EDITION ===" << std::endl;
    std::cout << "Features: 3 Levels, Score System with Penalties" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  SPACE or 1-9: Select car" << std::endl;
    std::cout << "  Arrow Keys: Move selected car" << std::endl;
    std::cout << "  P or ESC: Pause/Menu" << std::endl;
    std::cout << "  R: Restart level (after win/lose)" << std::endl;
    std::cout << "Score: -10 for each collision!" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        if (gameState == PLAYING) {
            gameTime -= deltaTime;
            if (gameTime <= 0.0f) {
                gameTime = 0.0f;
                gameState = GAME_OVER;
                std::cout << "TIME'S UP! Game Over. Final Score: " << score << std::endl;
            }
        }

        glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gameState == PLAYING || gameState == PAUSED || gameState == WIN || gameState == GAME_OVER) {
            glUseProgram(shaderProgram);

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                                    (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                    0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            drawParkingLot(shaderProgram, VAO, VBO);
            
            for (size_t i = 0; i < cars.size(); i++) {
                drawCar(shaderProgram, VAO, VBO, cars[i], i == selectedCarIndex);
            }
        }

        glDisable(GL_DEPTH_TEST);
        glUseProgram(shader2D);
        
        glm::mat4 projection2D = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f);
        unsigned int proj2DLoc = glGetUniformLocation(shader2D, "projection");
        glUniformMatrix4fv(proj2DLoc, 1, GL_FALSE, glm::value_ptr(projection2D));

        if (gameState == MENU) {
            drawText(shader2D, VAO2D, VBO2D, "3D PARKING JAM", 280, 120, 7.0f, glm::vec3(1.0f, 0.5f, 0.0f));
            
            for (const auto& btn : menuButtons) {
                drawButton(shader2D, VAO2D, VBO2D, btn);
            }
            
            drawText(shader2D, VAO2D, VBO2D, "USE ARROWS TO MOVE CARS", 340, 550, 2.5f, glm::vec3(0.8f, 0.8f, 0.8f));
            drawText(shader2D, VAO2D, VBO2D, "PRESS 1-9 TO SELECT CAR", 340, 590, 2.5f, glm::vec3(0.8f, 0.8f, 0.8f));
            drawText(shader2D, VAO2D, VBO2D, "AVOID COLLISIONS", 410, 630, 2.5f, glm::vec3(0.8f, 0.8f, 0.8f));
            
        } else if (gameState == LEVEL_SELECT) {
            drawText(shader2D, VAO2D, VBO2D, "SELECT LEVEL", 360, 120, 6.0f, glm::vec3(0.0f, 0.8f, 1.0f));
            
            for (const auto& btn : levelButtons) {
                drawButton(shader2D, VAO2D, VBO2D, btn);
            }
            
            drawText(shader2D, VAO2D, VBO2D, "EASY", 365, 380, 2.5f, glm::vec3(0.5f, 1.0f, 0.5f));
            drawText(shader2D, VAO2D, VBO2D, "MEDIUM", 595, 380, 2.5f, glm::vec3(1.0f, 1.0f, 0.5f));
            drawText(shader2D, VAO2D, VBO2D, "HARD", 865, 380, 2.5f, glm::vec3(1.0f, 0.5f, 0.5f));
            
        } else if (gameState == PLAYING) {
            std::stringstream timeStr;
            int minutes = (int)gameTime / 60;
            int seconds = (int)gameTime % 60;
            timeStr << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
            drawText(shader2D, VAO2D, VBO2D, "TIME " + timeStr.str(), 20, 20, 4.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            
            std::stringstream scoreStr;
            scoreStr << score;
            drawText(shader2D, VAO2D, VBO2D, "SCORE " + scoreStr.str(), 20, 70, 4.0f, glm::vec3(1.0f, 1.0f, 0.0f));
            
            std::stringstream levelStr;
            levelStr << "LEVEL " << currentLevel;
            drawText(shader2D, VAO2D, VBO2D, levelStr.str(), 20, 120, 3.5f, glm::vec3(0.5f, 1.0f, 1.0f));
            
            std::stringstream carStr;
            carStr << "CAR " << (selectedCarIndex + 1);
            drawText(shader2D, VAO2D, VBO2D, carStr.str(), SCR_WIDTH - 220, 20, 4.0f, glm::vec3(0.5f, 1.0f, 0.5f));
            
            drawText(shader2D, VAO2D, VBO2D, "P PAUSE", SCR_WIDTH - 190, 70, 3.0f, glm::vec3(0.7f, 0.7f, 0.7f));
            
        } else if (gameState == PAUSED) {
            drawRect(shader2D, VAO2D, VBO2D, 0, 0, SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f));
            
            drawText(shader2D, VAO2D, VBO2D, "PAUSED", 450, 150, 6.0f, glm::vec3(1.0f, 1.0f, 0.0f));
            
            for (const auto& btn : pauseButtons) {
                drawButton(shader2D, VAO2D, VBO2D, btn);
            }
            
        } else if (gameState == WIN) {
            drawText(shader2D, VAO2D, VBO2D, "YOU WIN", 380, 200, 8.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            
            std::stringstream finalScore;
            finalScore << "SCORE " << score;
            drawText(shader2D, VAO2D, VBO2D, finalScore.str(), 400, 350, 5.0f, glm::vec3(1.0f, 1.0f, 0.0f));
            
            drawText(shader2D, VAO2D, VBO2D, "R TO RESTART", 400, 480, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
            drawText(shader2D, VAO2D, VBO2D, "ESC TO MENU", 410, 520, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
            
        } else if (gameState == GAME_OVER) {
            drawText(shader2D, VAO2D, VBO2D, "GAME OVER", 330, 200, 7.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            
            std::stringstream finalScore;
            finalScore << "SCORE " << score;
            drawText(shader2D, VAO2D, VBO2D, finalScore.str(), 400, 350, 5.0f, glm::vec3(1.0f, 1.0f, 0.0f));
            
            drawText(shader2D, VAO2D, VBO2D, "R TO RESTART", 400, 480, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
            drawText(shader2D, VAO2D, VBO2D, "ESC TO MENU", 410, 520, 3.0f, glm::vec3(0.8f, 0.8f, 0.8f));
        }

        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO2D);
    glDeleteBuffers(1, &VBO2D);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shader2D);

    glfwTerminate();
    return 0;
}