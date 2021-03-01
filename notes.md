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
- vertex `(x,y,z)`
- primitive: simple shape defined by vertices
- set up data of vertices for the primitives we want to render
- done in the application itself
- uses VAOs (vertex array objects, defines what data a vertex has) and VBOs (vertex buffer objects, defines the data itself)
- attribute pointers define where and how shaders can access vertex data

    i. create VAO/VBO  
    ii. initiate draw

2. Vertex shader (programmable)
- handles vertices individually
- not optional
- can specify additional outputs
- inputs consist of the vertex data itself

3. Tessellation (programmable)
- allows you to divide data in to smaller primitives
- can be used to add higher levels of detail dynamically

4. Geometry shade (programmable)
- handles primitives
- takes primitives then "emits" their vertices to create the given primitives
- can create and modify primitives
- can alter primitive type

5. Vertex post-processing  
    i. Tranform Feedback
    - result of vertex and geometry stages saved to buffers for later use   

    ii. Clipping
    - primitive that won't be visible are removed
    - positions converted from "clip-space" to "window space"

6. Primitive assembly
- vertices converted in to a series of primitives
- face culling: removal of primitives that can't be seen

7. Rasterization
- converts primitives to "fragments"
- fragments are pieces of data for each pixel
- fragment data will be interpolated based on its position relative to each vertex

8. Fragment shader (programmable)
- handles data for each fragment
- optional but it's rare not to use it (exceptions are cases where only depth or stencil data is required)
- most important output is the color of the pixel that the fragment covers

9. Per-sample operations
- series of tests run to see if the fragment should be drawn
- most important test: DEPTH TEST (determines if something is in front of the point being drawn)
- color blending: overlapping fragments - used to handle transparency
- fragment data written to currently bound framebuffer
- lastly, buffer swap

### Shader programs
- Created with at least a vertex shader and then activated before use

---

## Vectors, matrices and uniform variables

### Vectors
- some basic definitions and operations
- dot product: `v1·v2 = |v1|*|v2|*cos(θ)`
- cross product only works in 3d

### Matrices
- some basic definitions and operations
- matrix transformations (translation, rotation, scaling)
1. **Translation**  
    - changing of `(X,Y,Z)` position of point `(x,y,z)`
    ```
    _          _   _ _   _     _  
    |1  0  0  X|   |x|   |x + X|  
    |0  1  0  Y| . |y| = |y + Y|  
    |0  0  1  Z|   |z|   |z + Z|  
    |0  0  0  1|   |1|   |  1  |
    -          -   - -   -     -  
    ```
2. Scaling
    - resizes vector by a factor of S
    ```
    _          _    _ _   _    _  
    |SX  0  0  X|   |x|   |SX·x|  
    |0  SY  0  Y| . |y| = |SY·y|  
    |0  0  SZ  Z|   |z|   |SZ·z|  
    |0  0  0   1|   |1|   |  1 |
    -          -    - -   -    -  
    ```
3. Rotation
    - rotates vector around its origin

    i. x rotation
    ```
    _                 _   _ _   _               _  
    |1   0      0    0|   |x|   |       x       |  
    |0  cosθ  -sinθ  0| . |y| = |cosθ·y - sinθ·z|  
    |0  sinθ   cosθ  0|   |z|   |sinθ·y + cosθ·z|  
    |0   0      0    1|   |1|   |       1       |
    -                 -   - -   -               -  
    ```
    ii. y rotation
    ```
    _                 _   _ _   _                _  
    | cosθ  0  sinθ  0|   |x|   | cosθ·x + sinθ·z|  
    |  0    1   0    0| . |y| = |        y       |  
    |-sinθ  0  cosθ  0|   |z|   |-sinθ·x + cosθ·z|  
    |  0    0   0    1|   |1|   |        1       |
    -                 -   - -   -                -  
    ```
    iii. z rotation
    ```
    _                  _   _ _   _               _  
    |cosθ   -sinθ  0  0|   |x|   |cosθ·x - sinθ·y|  
    |sinθ    cosθ  0  0| . |y| = |sinθ·x + cosθ·y|  
    | 0       0    1  0|   |z|   |       z       |  
    | 0       0    0  1|   |1|   |       1       |
    -                  -   - -   -               -  
    ```
- to combine transforms, just multiple them. then apply to the vector
- ORDER MATTERS: transforms happen in reverse order

### GLM (OpenGL Mathematics)
- uses `vec4` (vectors with 4 values) and `mat4` (4x4 matrix) types
- **installing**  
    ```bash
    wget "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip"
    unzip glm-0.9.9.8.zip
    cd glm-0.9.9.8
    cd glm
    cmake ./
    make
    sudo make install
    ```
    or
    ```bash
    sudo apt install libglm-dev
    ```

### Uniform variables
- type of a variable in shader
- uniforms are values global to the shader that aren't associated with a particular vertex
- each uniform variable has a location ID in the shader
- we can bind a value to that location
- **Differences in the context of shade:**  
    | attribute variables | uniform variables |
    |-|-|
    | distinct for each point pass through | we pass one variable and it will be the same on every single instance |
    | for each time the shader runs, the position will change | is constant over the whole flow |

### View, model and projection
- 3 main matrices: view, model and projection
- different coordinate systems
- model: builds from (0,0) - around the origin - to the actual world coordinates
- view:
- projection: perspective matrix
