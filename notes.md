# Course Notes

## **Section 1: Introduction**

## GLEW, GLFW and SDL

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

---

## **Section 2: Beginner**

## Shaders and the rendering pipeline
- Rendering pipeline: series of stages
- 4 stages: programmable via shaders
- shaders are piece of code written in CLSL (OpenGL Shading Language): in C

### The rendering pipeline stages

1. Vertex specification  
- vertex $(x,y,z)$
- primitive: simple shape defined by vertices
- set up data of vertices for the primitives we want to render
- done in the application itself
- uses VAOs (vertex array objects, defines what data a vertex has) and VBOs (vertex buffer objects, defines the data itself)
- attribute pointers define where and how shaders can access vertex data
    1. create VAO/VBO
    1. initiate draw

1. Vertex shader (programmable)
- handles vertices individually
- not optional
- can specify additional outputs
- inputs consist of the vertex data itself

1. Tessellation (programmable)
- allows you to divide data in to smaller primitives
- can be used to add higher levels of detail dynamically

1. Geometry shade (programmable)
- handles primitives
- takes primitives then "emits" their vertices to create the given primitives
- can create and modify primitives
- can alter primitive type

1. Vertex post-processing
    1. Tranform Feedback
    - result of vertex and geometry stages saved to buffers for later use
    1. Clipping
    - primitive that won't be visible are removed
    - positions converted from "clip-space" to "window space"

1. Primitive assembly
- vertices converted in to a series of primitives
- face culling: removal of primitives that can't be seen

1. Rasterization
- converts primitives to "fragments"
- fragments are pieces of data for each pixel
- fragment data will be interpolated based on its position relative to each vertex

1. Fragment shader (programmable)
- handles data for each fragment
- optional but it's rare not to use it (exceptions are cases where only depth or stencil data is required)
- most important output is the color of the pixel that the fragment covers

1. Per-sample operations
- series of tests run to see if the fragment should be drawn
- most important test: DEPTH TEST (determines if something is in front of the point being drawn)
- color blending: overlapping fragments - used to handle transparency
- fragment data written to currently bound framebuffer
- lastly, buffer swap

### Shader programs
- Created with at least a vertex shader and then activated before use
