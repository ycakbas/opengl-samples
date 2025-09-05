#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdbool.h>

static GLFWwindow *window;
static int width = 640;
static int height = 480;

void init() {
    // INIT

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "initialization", error);
    }

    // TEST 1: GL_NO_ERROR

    printf("--- Triggering GL_NO_ERROR ---\n");
    // check_gl_error("clearing previous errors");
    error = glGetError();
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "clearing previous errors", error);
    }
    error = glGetError();
    if (error == GL_NO_ERROR) {
        printf("Successfully received GL_NO_ERROR (0x%04X)\n\n", error);
    } else {
        fprintf(stderr, "Failed to get GL_NO_ERROR. Received 0x%04X instead.\n\n", error);
    }

    // TEST 2: GL_INVALID_ENUM

    printf("--- Triggering GL_INVALID_ENUM ---\n");
    glEnable(0); // '0' is not a valid capability enum.
    error = glGetError();
    if (error == GL_INVALID_ENUM) {
        printf("Successfully triggered GL_INVALID_ENUM (0x%04X)\n\n", error);
    } else {
        fprintf(stderr, "Failed to trigger GL_INVALID_ENUM. Got 0x%04X instead.\n\n", error);
    }
    // check_gl_error("trigger_gl_invalid_enum cleanup");
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "trigger_gl_invalid_enum cleanup", error);
    }

    // TEST 3: GL_INVALID_VALUE

    printf("--- Triggering GL_INVALID_VALUE ---\n");
    glLineWidth(0.0f); // Non-positive width is an invalid value.
    error = glGetError();
    if (error == GL_INVALID_VALUE) {
        printf("Successfully triggered GL_INVALID_VALUE (0x%04X)\n\n", error);
    } else {
        fprintf(stderr, "Failed to trigger GL_INVALID_VALUE. Got 0x%04X instead.\n\n", error);
    }
    // check_gl_error("trigger_gl_invalid_value cleanup");
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "trigger_gl_invalid_value cleanup", error);
    }

    // TEST 4: GL_INVALID_OPERATION

    printf("--- Triggering GL_INVALID_OPERATION ---\n");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // This is the invalid operation: attaching a shader TO another shader.
    glAttachShader(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    error = glGetError();
    if (error == GL_INVALID_OPERATION) {
        printf("Successfully triggered GL_INVALID_OPERATION (0x%04X)\n\n", error);
    } else {
        fprintf(stderr, "Failed to trigger GL_INVALID_OPERATION. Got 0x%04X instead.\n\n", error);
    }
    // check_gl_error("trigger_gl_invalid_operation cleanup");
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "trigger_gl_invalid_operation cleanup", error);
    }

    // TEST 5: GL_OUT_OF_MEMORY

    printf("--- Triggering GL_OUT_OF_MEMORY ---\n");

    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    printf("Max texture size is %d x %d.\n", max_texture_size, max_texture_size);
    printf("Attempting to repeatedly allocate max-sized textures until memory is exhausted...\n");

    // Store successfully allocated textures for cleanup
    const int max_allocations = 128;
    GLuint allocated_textures[max_allocations];
    int allocated_count = 0;
    bool triggered = false;

    for (int i = 0; i < max_allocations; ++i) {
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        // Use valid dimensions, but try to allocate a massive amount of memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max_texture_size, max_texture_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        error = glGetError();
        if (error == GL_OUT_OF_MEMORY) {
            printf("Successfully triggered GL_OUT_OF_MEMORY (0x%04X) on attempt #%d\n\n", error, i + 1);
            triggered = true;
            glDeleteTextures(1, &texID); // Delete the handle for the failed allocation
            break;
        } else if (error == GL_NO_ERROR) {
            printf("Allocation #%d successful...\n", i + 1);
            allocated_textures[allocated_count++] = texID;
        } else {
            // Some other unexpected error
            fprintf(stderr, "Unexpected error 0x%04X on attempt #%d. Aborting test.\n", error, i + 1);
            glDeleteTextures(1, &texID);
            break;
        }
    }

    if (!triggered) {
        fprintf(stderr, "Failed to trigger GL_OUT_OF_MEMORY after %d attempts. The system may have too much VRAM or a lenient driver.\n\n", max_allocations);
    }

    // Cleanup all successfully allocated textures
    if (allocated_count > 0) {
        printf("Cleaning up %d successfully allocated texture(s)...\n", allocated_count);
        glDeleteTextures(allocated_count, allocated_textures);
        // check_gl_error("texture cleanup");
        while ((error = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "texture cleanup", error);
        }
    }

    // TEST 6: GL_INVALID_FRAMEBUFFER_OPERATION

    printf("--- Triggering GL_INVALID_FRAMEBUFFER_OPERATION ---\n");
    GLuint fbo, renderbuffer;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);

    // By not calling glRenderbufferStorage, we leave the framebuffer incomplete.
    glClear(GL_COLOR_BUFFER_BIT);

    error = glGetError();
    if (error == GL_INVALID_FRAMEBUFFER_OPERATION) {
        printf("Successfully triggered GL_INVALID_FRAMEBUFFER_OPERATION (0x%04X)\n\n", error);
    } else {
        fprintf(stderr, "Failed to trigger GL_INVALID_FRAMEBUFFER_OPERATION. Got 0x%04X instead.\n\n", error);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &renderbuffer);
    // check_gl_error("trigger_gl_invalid_framebuffer_operation cleanup");
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "After %s, OpenGL error: 0x%04X\n", "trigger_gl_invalid_framebuffer_operation cleanup", error);
    }
}

void draw() {
    // Nothing to draw
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "OpenGL ES 2.0 Error Test", NULL, NULL);
    if (!window) {
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
