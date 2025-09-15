//
// Created by Yusuf on 21.07.2025.
//

#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_SHADERS 5
static GLFWwindow *window;
static GLuint shaderPrograms[NUM_SHADERS];
static GLuint vbo;
static GLint posLoc;

// Embedded shader sources
static const char *fragcoord_frag =
    "#version 100\n"
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(gl_FragCoord.xy / 800.0, 0.0, 1.0);\n"
    "}\n";
static const char *frontfacing_frag =
    "#version 100\n"
    "precision mediump float;\n"
    "void main() {\n"
    "#ifdef GL_OES_standard_derivatives\n"
    "    if (gl_FrontFacing) {\n"
    "        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Green for front faces\n"
    "    } else {\n"
    "        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for back faces\n"
    "    }\n"
    "#else\n"
    "    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0); // Fallback for ES 2.0\n"
    "#endif\n"
    "}\n";
static const char *pointcoord_frag =
    "#version 100\n"
    "// Demonstrates gl_PointCoord usage (OpenGL ES 2.0)\n"
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(gl_PointCoord, 0.0, 1.0);\n"
    "}\n";
static const char *fragcolor_frag =
    "#version 100\n"
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(0.2, 0.4, 0.6, 1.0);\n"
    "}\n";
static const char *fragdata_frag =
    "#version 100\n"
    "precision mediump float;\n"
    "void main() {\n"
    "    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Only gl_FragColor is available in ES 2.0\n"
    "    gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0); // Not available in ES 2.0\n"
    "}\n";
static const char *pointsize_vert =
    "#version 100\n"
    "attribute vec3 aPosition;\n"
    "uniform float uPointSize;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPosition, 1.0);\n"
    "    gl_PointSize = uPointSize;\n"
    "}\n";

// Remove static/global initialization of frag_shaders
// Instead, declare as NULL and initialize in main before use
static const char *frag_shaders[NUM_SHADERS] = {NULL};

// Modified compile_shader to accept source string directly
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

// Modified create_shader_program to use embedded sources
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
    // Initialize frag_shaders array after all shader strings are defined
    frag_shaders[0] = fragcoord_frag;
    frag_shaders[1] = frontfacing_frag;
    frag_shaders[2] = pointcoord_frag;
    frag_shaders[3] = fragcolor_frag;
    frag_shaders[4] = fragdata_frag;

    // Vertex data for a triangle
    float vertices[3][3] = {
        {0.0f, 0.8f, 0.0f},
        {-0.8f, -0.8f, 0.0f},
        {0.8f, -0.8f, 0.0f}};
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Compile all shader programs once and store them
    for (int i = 0; i < NUM_SHADERS; ++i)
    {
        shaderPrograms[i] = create_shader_program_embedded(pointsize_vert, frag_shaders[i]);
    }
    // Use the first program to get the attribute location (all use same vertex shader)
    posLoc = glGetAttribLocation(shaderPrograms[0], "aPosition");
}

void draw()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    int viewport_width = width / NUM_SHADERS;
    for (int i = 0; i < NUM_SHADERS; i++)
    {
        if (!shaderPrograms[i])
            continue;
        glUseProgram(shaderPrograms[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glViewport(i * viewport_width, 0, viewport_width, height);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(posLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

int main(void)
{
    if (!glfwInit())
    {
        printf("ERROR: Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(800, 600, "Fragment Shader Built-in Variables Example", NULL, NULL);
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
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
