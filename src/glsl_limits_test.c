// glsl_limits_test.c
// Test OpenGL ES 2.0 built-in constant limits
#include <GLFW/glfw3.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>

static GLFWwindow *window;
static GLuint shaderProgram;
static GLuint vbo;
static GLint posLoc;
static GLint uIndexLoc;

// Embedded shader sources
static const char *glsl_limits_test_vert =
    "#version 100\n"
    "\n"
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "    gl_Position = a_position;\n"
    "}\n";
static const char *glsl_limits_test_frag =
    "#version 100\n"
    "\n"
    "precision mediump int;\n"
    "precision mediump float;\n"
    "\n"
    "uniform int u_index;\n"
    "\n"
    "void main() {\n"
    "    int value = 0;\n"
    "    int minValue = 0;\n"
    "    if (u_index == 0) {\n"
    "        value = gl_MaxVertexAttribs;\n"
    "        minValue = 8;\n"
    "    } else if (u_index == 1) {\n"
    "        value = gl_MaxVertexUniformVectors;\n"
    "        minValue = 128;\n"
    "    } else if (u_index == 2) {\n"
    "        value = gl_MaxVaryingVectors;\n"
    "        minValue = 8;\n"
    "    } else if (u_index == 3) {\n"
    "        value = gl_MaxVertexTextureImageUnits;\n"
    "        minValue = 0;\n"
    "    } else if (u_index == 4) {\n"
    "        value = gl_MaxCombinedTextureImageUnits;\n"
    "        minValue = 8;\n"
    "    } else if (u_index == 5) {\n"
    "        value = gl_MaxTextureImageUnits;\n"
    "        minValue = 8;\n"
    "    } else if (u_index == 6) {\n"
    "        value = gl_MaxFragmentUniformVectors;\n"
    "        minValue = 16;\n"
    "    } else if (u_index == 7) {\n"
    "        value = gl_MaxDrawBuffers;\n"
    "        minValue = 1;\n"
    "    }\n"
    "    if (value >= minValue) {\n"
    "        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); // green\n"
    "    } else {\n"
    "        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // red\n"
    "    }\n"
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
    // Request OpenGL ES 2.0 context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(800, 600, "GLSL Constants With Min Values Test", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    shaderProgram = create_shader_program_embedded(glsl_limits_test_vert, glsl_limits_test_frag);
    glUseProgram(shaderProgram);
    uIndexLoc = glGetUniformLocation(shaderProgram, "u_index");
    float vertices[] = {
        -0.8f, -0.8f,
        0.8f, -0.8f,
        0.0f, 0.8f};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    posLoc = glGetAttribLocation(shaderProgram, "a_position");
}

void draw()
{
    int win_w, win_h;
    glfwGetFramebufferSize(window, &win_w, &win_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    for (int i = 0; i < 8; i++)
    {
        int col = i % 4, row = i / 4;
        int vp_w = win_w / 4, vp_h = win_h / 2;
        glViewport(col * vp_w, row * vp_h, vp_w, vp_h);
        glUniform1i(uIndexLoc, i);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glDisableVertexAttribArray(posLoc);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void cleanup()
{
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }
    init();
    while (!glfwWindowShouldClose(window))
    {
        draw();
    }
    cleanup();
    return 0;
}
