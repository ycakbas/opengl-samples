#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static GLFWwindow *window;
static GLuint shaderProgram;
static GLint uPointSizeLoc;

static GLuint vbo;
static GLint posLoc;

// Embedded shader sources
static const char *pointsize_vert =
    "#version 100\n"
    "attribute vec3 aPosition;\n"
    "uniform float uPointSize;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPosition, 1.0);\n"
    "    gl_PointSize = uPointSize;\n"
    "}\n";
static const char *pointsize_frag =
    "#version 100\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

GLuint compile_shader_from_source(const char *source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    if (!shader)
    {
        printf("ERROR: Failed to create shader object\n");
        return 0;
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char *infoLog = (char *)malloc(logLength);
        if (infoLog)
        {
            glGetShaderInfoLog(shader, logLength, NULL, infoLog);
            printf("ERROR: Shader compilation failed: %s\n", infoLog);
            free(infoLog);
        }
        else
        {
            printf("ERROR: Shader compilation failed. (Could not allocate infoLog)\n");
        }
        glDeleteShader(shader);
        return 0;
    }
    printf("INFO: Shader compiled successfully\n");
    return shader;
}

GLuint create_shader_program_embedded(const char *vertex_src, const char *fragment_src)
{
    GLuint vertexShader = compile_shader_from_source(vertex_src, GL_VERTEX_SHADER);
    if (!vertexShader)
    {
        printf("ERROR: Vertex shader compilation failed\n");
        return 0;
    }
    GLuint fragmentShader = compile_shader_from_source(fragment_src, GL_FRAGMENT_SHADER);
    if (!fragmentShader)
    {
        printf("ERROR: Fragment shader compilation failed\n");
        glDeleteShader(vertexShader);
        return 0;
    }
    GLuint program = glCreateProgram();
    if (!program)
    {
        printf("ERROR: Failed to create shader program\n");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        char *infoLog = (char *)malloc(logLength);
        if (infoLog)
        {
            glGetProgramInfoLog(program, logLength, NULL, infoLog);
            printf("ERROR: Program linking failed: %s\n", infoLog);
            free(infoLog);
        }
        else
        {
            printf("ERROR: Program linking failed. (Could not allocate infoLog)\n");
        }
        glDeleteProgram(program);
        program = 0;
    }
    else
    {
        printf("INFO: Shader program linked successfully\n");
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void init()
{
    shaderProgram = create_shader_program_embedded(pointsize_vert, pointsize_frag);
    uPointSizeLoc = glGetUniformLocation(shaderProgram, "uPointSize");
    float points[4][3] = {
        {-0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.0f},
        {-0.2f, -0.2f, 0.0f},
        {0.2f, -0.2f, 0.0f}};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    posLoc = glGetAttribLocation(shaderProgram, "aPosition");
}

void draw()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    float sizes[3] = {10.0f, 30.0f, 60.0f};
    for (int i = 0; i < 3; ++i)
    {
        glViewport(i * width / 3, 0, width / 3, height);
        glUniform1f(uPointSizeLoc, sizes[i]);
        glDrawArrays(GL_POINTS, 0, 4);
    }
    glDisableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Swap front and back buffers
    glfwSwapBuffers(window);
    // Poll for and process events
    glfwPollEvents();
}

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        printf("ERROR: Failed to initialize GLFW\n");
        return -1;
    }

    // Set GLFW window hints for OpenGL ES 2.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1200, 800, "gl_Position and gl_PointSize Example", NULL, NULL);
    if (!window)
    {
        printf("ERROR: Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    init();
    while (!glfwWindowShouldClose(window))
    {
        draw();
    }

    return 0;
}
