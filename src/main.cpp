#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int gridSize = 8;
const float rectSize = 2.0f / gridSize;
const float rectWidth = rectSize * 1.5f;
const float rectHeight = rectSize * 0.8f;

struct Rect {
    float r, g, b;
    bool active;
};

Rect grid[gridSize][gridSize];

int totalScore = 0;
int currentTryScore = 100;

GLuint shaderProgram;
GLuint VAO, VBO, EBO;

float vertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

unsigned int indices[] = {
    0,1,2,
    2,3,0
};

std::string loadShader(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        std::cerr << "Erro ao abrir shader: " << path << std::endl;
        exit(-1);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* buffer = new char[size + 1];
    buffer[size] = 0;
    fread(buffer, 1, size, file);
    fclose(file);

    std::string code(buffer);
    delete[] buffer;
    return code;
}

GLuint compileShader(const char* path, GLenum type) {
    std::string code = loadShader(path);
    const char* ccode = code.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &ccode, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Erro compilando shader: " << path << "\n" << infoLog << std::endl;
        exit(-1);
    }
    return shader;
}

void initGrid() {
    srand(time(NULL));
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j].r = (float)(rand() % 256) / 255.0f;
            grid[i][j].g = (float)(rand() % 256) / 255.0f;
            grid[i][j].b = (float)(rand() % 256) / 255.0f;
            grid[i][j].active = true;
        }
    }
    totalScore = 0;
    currentTryScore = 100;
}

float colorDistance(float r1, float g1, float b1, float r2, float g2, float b2) {
    float dr = r1 - r2;
    float dg = g1 - g2;
    float db = b1 - b2;
    return sqrt(dr*dr + dg*dg + db*db);
}

bool isGameOver() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid[i][j].active)
                return false;
        }
    }
    return true;
}


void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    int colorLoc = glGetUniformLocation(shaderProgram, "uColor");
    glUniform3f(colorLoc, r, g, b);

    int offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    glUniform2f(offsetLoc, x, y);

    int scaleLoc = glGetUniformLocation(shaderProgram, "uScale");
    glUniform2f(scaleLoc, width, height);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float x = (float)(xpos / width) * 2.0f - 1.0f;
        float y = 1.0f - (float)(ypos / height) * 2.0f;

        int col = (int)((x + 1.0f) / rectWidth);
        int row = (int)((y + 1.0f) / rectHeight);

        if (row < 0 || row >= gridSize || col < 0 || col >= gridSize) return;
        if (!grid[row][col].active) return;

        float rClick = grid[row][col].r;
        float gClick = grid[row][col].g;
        float bClick = grid[row][col].b;

        const float threshold = 0.3f;

        int removedCount = 0;

        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                if (grid[i][j].active) {
                    float dist = colorDistance(rClick, gClick, bClick, grid[i][j].r, grid[i][j].g, grid[i][j].b);
                    if (dist < threshold) {
                        grid[i][j].active = false;
                        removedCount++;
                    }
                }
            }
        }

        if (removedCount > 0) {
            totalScore += currentTryScore * removedCount;
            currentTryScore = std::max(10, currentTryScore - 10);
            std::cout << "Removidos: " << removedCount << " | Pontuacao total: " << totalScore << std::endl;
        }
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        initGrid();
        std::cout << "Jogo reiniciado!" << std::endl;
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Jogo das Cores", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar janela GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD\n";
        return -1;
    }

    GLuint vertexShader = compileShader("shaders/vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    initGrid();

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
 

         for (int i = 0; i < gridSize; i++) {
             for (int j = 0; j < gridSize; j++) {
                 if (grid[i][j].active) {
                 float x = -1.0f + j * rectWidth;
                 float y = -1.0f + i * rectHeight;

                 drawRect(x, y, rectWidth, rectHeight, grid[i][j].r, grid[i][j].g, grid[i][j].b);
                 }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    if (isGameOver()) {
        std::cout << "=== Parabens!! Voce concluiu o jogo ===\nPontuacao final: " << totalScore << std::endl;
        glfwSetWindowShouldClose(window, true); // fecha o jogo automaticamente
}

    }

    glfwTerminate();
    return 0;
}