#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformModel;

// Uniform variables
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;

// Vertex Shader (should be in another file)
static const char* vShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
layout (location=0) in vec3 pos; // in:input                    \n\
                                                                \n\
uniform mat4 model;                                             \n\
                                                                \n\
void main() {                                                   \n\
    gl_Position = model * vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);   \n\
}";

// Fragment Shader
static const char* fShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
out vec4 color;                                                 \n\
                                                                \n\
void main() {                                                   \n\
    color = vec4(1.0, 0.0, 0.0, 1.0);                           \n\
}";

void CreateTriangle() {
    // window axis:
    //    __________
    //   |     1    |
    //   |-1       1|
    //   |____-1____|

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO); // &VAO is the id
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO); // &VBO is the id
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind

    glBindVertexArray(0); // unbind
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    // error logs, to avoid debugging
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s' \n", shaderType, eLog);
        return;
    }

    // finally attaching shader to the program
    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program!\n");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    // error logs, to avoid debugging
    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s' \n", eLog);
        return;
    }

    // validations
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

    uniformModel= glGetUniformLocation(shader, "model"); // from layout location=0
}

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

    // Now we're finally drawing!
    CreateTriangle();
    CompileShaders();

    // Loop until window closed
    while (!glfwWindowShouldClose(mainWindow)) {
        
        // Get + handle user input events
        glfwPollEvents();

        // Moving triangle (using uniform variables!)
        if (direction) { // right
            triOffset += triIncrement;
        } else { // left
            triOffset -= triIncrement;
        }

        // Changing triangle direction
        if (abs(triOffset) >= triMaxoffset) {
            direction = !direction;
        }

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGB -> [0,1], opacity (alpha)
        glClear(GL_COLOR_BUFFER_BIT);

        // Shaders and rendering pipeline
        glUseProgram(shader);

        // 4x4 identity matrix
        glm::mat4 model(1.0f);
        // applies translation to identity matrix (in the x AND y axis)
        model = glm::translate(model, glm::vec3(triOffset, triOffset, 0.0f));

        // 4x4 float values matrix
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO); // bind
        glDrawArrays(GL_TRIANGLES, 0, 3); // start from point 0, 3 points
        glBindVertexArray(VAO); // unbind

        glUseProgram(0); // close shader

        glfwSwapBuffers(mainWindow); // 2 scenes: drawing and user
    }

    return 0;
}
