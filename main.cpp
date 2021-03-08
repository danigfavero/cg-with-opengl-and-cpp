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
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

// Uniform variables
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
static const char* vShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
layout (location=0) in vec3 pos; // in:input                    \n\
                                                                \n\
out vec4 vColor;                                                \n\
                                                                \n\
uniform mat4 model;                                             \n\
uniform mat4 projection;                                        \n\
                                                                \n\
void main() {                                                   \n\
    gl_Position = projection * model * vec4(pos, 1.0);          \n\
    vColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);                \n\
}";

// Fragment Shader
static const char* fShader = "                                  \n\
#version 330                                                    \n\
                                                                \n\
in vec4 vColor;                                                 \n\
                                                                \n\
out vec4 color;                                                 \n\
                                                                \n\
void main() {                                                   \n\
    color = vColor;                                             \n\
}";

void CreateTriangle() {
    unsigned int indices[] = { // the indices refer to the vertices array
        0, 3, 1, // side of pyramid
        1, 3, 2, // another side
        2, 3, 0, // front face
        0, 1, 2  // base of pyramid
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f, // 4th point to the pyramid
        1.0, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO); // &VAO is the id
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO); // binding IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO); // &VBO is the id
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind

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

    uniformModel = glGetUniformLocation(shader, "model"); // from layout location=0
    uniformProjection = glGetUniformLocation(shader, "projection");
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

    // Adds depth to 3D image
    glEnable(GL_DEPTH_TEST);

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    // Now we're finally drawing!
    CreateTriangle();
    CompileShaders();

    // Projection
    glm::mat4 projection(1.0f);
    projection = glm::perspective(45.0f, (GLfloat)bufferWidth/(GLfloat)bufferHeight, 0.1f, 100.0f);

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

        curAngle += 0.5f;
        if (curAngle >= 360) {
            curAngle -= 360;
        }

        if (sizeDirection) { // enlarge!
            curSize += 0.001f;
        } else { // shrink!
            curSize -= 0.001f;
        }

        if (curSize >= maxSize || curSize <= minSize) { // flip it!
            sizeDirection = !sizeDirection;
        }

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // RGB -> [0,1], opacity (alpha)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Shaders and rendering pipeline
        glUseProgram(shader);

        // 4x4 identity matrix
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

        // 4x4 float values matrix
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO); // bind
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0); // unbind

        glUseProgram(0); // close shader

        glfwSwapBuffers(mainWindow); // 2 scenes: drawing and user
    }

    return 0;
}
