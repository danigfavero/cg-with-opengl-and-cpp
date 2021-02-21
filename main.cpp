#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

int main() {
    
    // Initialise GLFW
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = No backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    // Loop until window closed
    while (!glfwWindowShouldClose(mainWindow)) {
        
        // Get + handle user input events
        glfwPollEvents();

        // Clear window (red)
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // RGB -> [0,1], opacity (alpha)
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(mainWindow); // 2 scenes: drawing and user
    }

    return 0;
}
