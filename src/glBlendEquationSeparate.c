#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

static GLFWwindow *window;
static GLuint shaderProgram;
static GLuint vertexBuffer;
static int width = 1200;
static int height = 800;

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
}

void draw() {
    glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    int columnCount = 4;
    int rowCount = 3;
    GLenum equations[3] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT};

    glUseProgram(shaderProgram);

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            int x = col * width / columnCount;
            int y = row * height / rowCount;
            int w = width / columnCount;
            int h = height / rowCount;
            glViewport(x, y, w, h);
            if (col == 0) {
                glDisable(GL_BLEND);
            } else {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(equations[row], equations[col - 1]);
            }
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawArrays(GL_TRIANGLES, 3, 3);
        }
    }
    glDisable(GL_BLEND);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, "glBlendEquation", NULL, NULL);
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
