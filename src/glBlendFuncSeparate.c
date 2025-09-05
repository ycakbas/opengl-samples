#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

static GLFWwindow *window;
static GLuint shaderProgram;
static GLuint vertexBuffer;
static int width = 1200;
static int height = 1200;

static GLenum glBlendEquationOptions[] = {
        GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT
};

static GLenum glBlendFuncSFactorOptions[] = {
        GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_SRC_ALPHA_SATURATE
};

static GLenum glBlendFuncDFactorOptions[] = {
        GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA
};

static GLenum glBlendEquationMode = GL_FUNC_ADD;
static GLenum glBlendFuncsSFactorAlpha = GL_ZERO;
static GLenum glBlendFuncsDFactorRGB = GL_ZERO;
static GLenum glBlendFuncsDFactorAlpha = GL_ZERO;

static int sFactorAlphaCount = sizeof(glBlendFuncSFactorOptions) / sizeof(GLenum);
static int dFactorRGBCount = sizeof(glBlendFuncDFactorOptions) / sizeof(GLenum);
static int dFactorAlphaCount = sizeof(glBlendFuncDFactorOptions) / sizeof(GLenum);

static double lastTime;
static int comboIndex = 0;

void init() {
    const char *vertexShaderSource =
        "attribute vec4 aPos;\n"
        "void main()\n"
        "{\n"
            "gl_Position = aPos;\n"
        "}\n";

    const char *fragmentShaderSource =
        "precision mediump float;\n"
        "void main()\n"
        "{\n"
            "gl_FragColor = vec4(0.2f, 0.4f, 0.6f, 0.5f);\n"
        "}\n";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glDeleteShader(vertexShader);
        vertexShader = 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glDeleteShader(fragmentShader);
        fragmentShader = 0;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = {
            -0.6f, -0.5f, 0.0f,  // left
            0.4f, -0.5f, 0.0f,   // right
            -0.1f, 0.5f, 0.0f,   // top

            -0.4f, -0.5f, 0.0f, // left
            0.6f, -0.5f, 0.0f, // right
            0.1f, 0.5f, 0.0f   // top
    };

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(posAttrib);

    lastTime = glfwGetTime();
}

void draw() {
    double currentTime = glfwGetTime();
    if (currentTime - lastTime >= 1.0) { // Change every second
        comboIndex++;
        int totalCombos = sFactorAlphaCount * dFactorRGBCount * dFactorAlphaCount;
        if (comboIndex >= totalCombos) comboIndex = 0;
        int sIdx = comboIndex % sFactorAlphaCount;
        int dRGBIdx = (comboIndex / sFactorAlphaCount) % dFactorRGBCount;
        int dAlphaIdx = (comboIndex / (sFactorAlphaCount * dFactorRGBCount)) % dFactorAlphaCount;
        glBlendFuncsSFactorAlpha = glBlendFuncSFactorOptions[sIdx];
        glBlendFuncsDFactorRGB = glBlendFuncDFactorOptions[dRGBIdx];
        glBlendFuncsDFactorAlpha = glBlendFuncDFactorOptions[dAlphaIdx];
        lastTime = currentTime;
        printf("Counter: %d | SFactorAlpha: %d, DFactorRGB: %d, DFactorAlpha: %d\n", comboIndex, glBlendFuncsSFactorAlpha, glBlendFuncsDFactorRGB, glBlendFuncsDFactorAlpha);
    }
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    int columnCount = 4;
    int rowCount = 4;

    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < columnCount; j++) {
            glViewport(j * width / columnCount, i * height / rowCount, width / columnCount, height / rowCount);
            glEnable(GL_BLEND);
            glBlendEquation(glBlendEquationMode);
            glBlendFuncSeparate(glBlendFuncSFactorOptions[(i * columnCount + j) % 14],
                                glBlendFuncsDFactorRGB,
                                glBlendFuncsSFactorAlpha,
                                glBlendFuncsDFactorAlpha
            );
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawArrays(GL_TRIANGLES, 3, 3);
        }
    }
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, "glBlendFunc", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    init();
    while (!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
