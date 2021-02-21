# Course Notes

## Section 1: Introduction

### GLEW, GLFW and SDL

- **GLEW: OpenGL Extension Wrangler**
    - interface for OpenGL
    - loads OpenGL extensions
    - handles platform-specific extensions safely
- **GLFW: OpenGL FrameWork**
    - window creation and control
    - pick up and process input
    - allows multiple monitor support
    - uses OpenGL context for windows
- **SDL: Simple DirectMedia Layer**
    - GLFW controls + audio, threading, filesystems

### Installing
- GLEW
    ```bash
    sudo apt-get install glew-utils
    ```
- GLFW
    ```bash
    sudo apt-get install cmake
    sudo apt-get build-dep glfw3
    wget "https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip"
    unzip glfw-3.3.2.zip
    cd glfw-3.3.2
    cmake -G "Unix Makefiles"
    make
    sudo make install
    ```

### Compiling
```bash
g++ -std=c++11 -c main.cpp
g++ -std=c++11 main.cpp -o out -lGLEW -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl
./out
```
**TODO:** a makefile...
