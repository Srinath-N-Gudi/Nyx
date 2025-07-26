# Nyx: A Lightweight OpenGL/GLFW Wrapper

Nyx is a C++ library designed to simplify OpenGL application development by providing a lightweight wrapper around GLFW and core OpenGL functionalities. It abstracts away much of the boilerplate code involved in window creation, input handling, and basic rendering setup, allowing developers to focus on the creative aspects of their graphics applications.

## ✨ Features

-   **Window Management**: Easy creation and management of GLFW windows with integrated OpenGL context handling.
    -   Configurable OpenGL versions (e.g., 3.3 Core Profile).
    -   Support for debug contexts.
    -   Resizable window control.
    -   Window state management (maximize, minimize, restore).
-   **Input Handling**: Seamless integration with GLFW's input system, accessible through the `Window` object.
-   **Renderer Abstractions**: Clean wrappers for common OpenGL objects:
    -   Vertex Array Objects (VAO)
    -   Vertex Buffer Objects (VBO)
    -   Index Buffer Objects (IBO)
-   **Shader Management**: Simplified loading and compilation of GLSL shaders.
-   **Texture Support**: Easy loading of 2D textures using the `stb_image.h` library.
-   **Modular Design**: A flexible and extensible architecture that allows for easy integration into existing projects.

## 🏛️ Architecture and Design Principles

Nyx is designed with modularity and ease of use in mind, providing a thin, object-oriented wrapper around core OpenGL and GLFW functionalities. The library's structure promotes clear separation of concerns, making it easier to understand, extend, and integrate into various projects.

### Core Components:

-   **`Window`**: This is the entry point for any Nyx application. It encapsulates GLFW window creation, OpenGL context management, and event polling. It also provides access to the `InputHandler`.

-   **`InputHandler`**: Responsible for abstracting keyboard and mouse input. It allows querying key and mouse button states, as well as mouse cursor position, without direct interaction with GLFW callbacks.

-   **`Image::Loader`**: A utility class for loading image data into `Texture2D` objects using `stb_image.h`. It simplifies the process of getting image assets into OpenGL textures.

-   **`Renderer::GL` Namespace**: This namespace contains all OpenGL-specific rendering abstractions. Each class within this namespace wraps a fundamental OpenGL object or concept:
    -   **`VAO` (Vertex Array Object)**: Manages the state of vertex attributes and their associated VBOs and IBOs. It defines how vertex data is interpreted by OpenGL.
    -   **`VBO` (Vertex Buffer Object)**: Stores vertex data (e.g., positions, colors, texture coordinates) on the GPU.
    -   **`IBO` (Index Buffer Object)**: Stores indices for indexed drawing, allowing for efficient rendering of shared vertices.
    -   **`Shader`**: Handles the compilation, linking, and activation of GLSL shader programs. It provides methods for setting uniform variables.
    -   **`Texture2D`**: Manages 2D OpenGL textures, including data upload, binding, and sampling parameters.
    -   **`Renderer`**: A higher-level abstraction for drawing multiple VAOs. It simplifies the drawing loop by managing a list of VAOs and providing an optional callback for per-VAO setup.

### Design Philosophy:

-   **Abstraction**: Nyx aims to abstract away the low-level complexities of OpenGL and GLFW, providing a more intuitive C++ interface.
-   **Modularity**: Each component is designed to be independent and focused on a single responsibility, allowing developers to use only the parts of Nyx they need.
-   **Performance**: While providing abstractions, Nyx tries to maintain performance close to raw OpenGL by offering direct access to underlying IDs (e.g., `getGLFWWindow()`, `getID()`) when necessary.
-   **Extensibility**: The design allows for easy extension and customization. For example, custom rendering pipelines can be built using the core `Renderer::GL` components.

## 🚀 Getting Started

To get started with Nyx, follow these steps:

1.  **Clone the Repository**:

    ```bash
    git clone https://github.com/Srinath-N-Gudi/Nyx.git
    ```

2.  **Add Dependencies**:

    Nyx relies on the following external libraries. You will need to set them up in your project:

    -   [**GLFW**](https://www.glfw.org/): For window creation and context management.
    -   [**GLAD**](https://glad.dav1d.de/) (or similar OpenGL loader like GLEW): Nyx expects an active OpenGL context, but does not include a loader. You must load OpenGL functions yourself after the window is created (e.g., `gladLoadGL();`).
    -   [**GLM**](https://github.com/g-truc/glm) (Optional): Used in examples for mathematics, but not a core dependency of Nyx itself.
    -   [`stb_image.h`](https://github.com/nothings/stb): Included in the `vendor/` directory for image loading.

3.  **Include Nyx in Your Project**:

    Include the necessary headers in your source files:

    ```cpp
    #include "Nyx/Window.h"
    #include "Nyx/Renderer/Shader.h"
    #include "Nyx/Renderer/Texture2D.h"
    // ... other Nyx components as needed
    ```

4.  **Example Usage**:

    A basic example demonstrating window creation and the main application loop:

    ```cpp
    #include <glad/glad.h> // Assuming GLAD is used
    #include "Nyx/Window.h"

    int main()
    {
        // Configure window settings (optional)
        Nyx::Window::WindowConfig config;
        config.glMajorVersion = 3;
        config.glMinorVersion = 3;
        config.coreProfile = true;
        config.debugContext = true; // Enable for OpenGL debug output
        config.resizable = true;

        // Create the window
        Nyx::Window::Window window("My Nyx Application", 1280, 720, config);

        // IMPORTANT: Load OpenGL functions after context creation
        gladLoadGL(); 

        // Set a resize callback (optional)
        window.setResizeCallback([](int width, int height) {
            glViewport(0, 0, width, height);
            std::cout << "Window resized to: " << width << "x" << height << std::endl;
        });

        // Main application loop
        while (!window.windowClosed())
        {
            // Clear the screen
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Your rendering code goes here

            // Update the window (swaps buffers and polls events)
            window.update();
        }

        return 0;
    }
    ```

## 📚 API Reference

### `Nyx::Window::Window`

The `Nyx::Window::Window` class is the core component for managing your application's window and OpenGL context.

#### `struct WindowConfig`

Used to configure the properties of the GLFW window and OpenGL context.

```cpp
struct WindowConfig {
    int glMajorVersion = 3;   // OpenGL major version (default: 3)
    int glMinorVersion = 3;   // OpenGL minor version (default: 3)
    bool coreProfile = true;  // Use core profile (true) or compatibility profile (false) (default: true)
    bool debugContext = false; // Enable OpenGL debug context (default: false)
    bool resizable = true;    // Allow window resizing (default: true)
};
```

#### Constructor

```cpp
Window(const char* windowTitle, int width, int height, const WindowConfig& config);
```

-   `windowTitle`: The title displayed in the window's title bar.
-   `width`: The initial width of the window in pixels.
-   `height`: The initial height of the window in pixels.
-   `config`: A `WindowConfig` struct to specify OpenGL context and window properties.

#### Destructor

```cpp
~Window();
```

-   Destroys the GLFW window and terminates GLFW.

#### Public Methods

-   `void update()`
    -   Swaps the front and back buffers and processes all pending GLFW events. Call this at the end of each frame.

-   `void setWindowTitle(const char* windowTitle)`
    -   Sets the title of the window.

-   `void setWidth(const int& width)`
    -   Sets the width of the window.

-   `void setHeight(const int& height)`
    -   Sets the height of the window.

-   `void setSize(const int& width, const int& height)`
    -   Sets both the width and height of the window.

-   `void setResizeCallback(const NyxResizeCallback& callback)`
    -   Sets a callback function to be invoked when the window is resized. The callback receives the new width and height.
    -   `NyxResizeCallback` is `std::function<void(int width, int height)>`.

-   `void getFramebufferSize(int& width, int& height) const`
    -   Retrieves the current width and height of the window's framebuffer.

-   `Nyx::InputHandler& getInputHandler()`
    -   Returns a reference to the `InputHandler` associated with this window, allowing access to keyboard and mouse input states.

-   `void maximize()`
    -   Maximizes the window to fill the screen.

-   `void minimize()`
    -   Minimizes (iconifies) the window.

-   `void restore()`
    -   Restores the window to its previous size and position if it was maximized or minimized.

#### Inline Getters

-   `inline int getWidth() const`
    -   Returns the current width of the window.

-   `inline int getHeight() const`
    -   Returns the current height of the window.

-   `inline const char* getWindowTitle() const`
    -   Returns the current title of the window.

-   `inline GLFWwindow* getGLFWWindow() const`
    -   Returns the raw `GLFWwindow` pointer, useful for direct GLFW API calls.

-   `inline bool windowClosed() const`
    -   Returns `true` if the window should close (e.g., user clicked the close button), `false` otherwise.

-   `inline bool isMinimized() const`
    -   Returns `true` if the window is currently minimized.

-   `inline bool isMaximized() const`
    -   Returns `true` if the window is currently maximized.

#### Operator Overload

-   `Nyx::InputHandler* operator->()`
    -   Allows direct access to the `InputHandler` methods using the arrow operator on the `Window` object (e.g., `window->isKeyPressed(GLFW_KEY_SPACE)`).

### `Nyx::Image::Loader`

The `Nyx::Image::Loader` class provides static methods for loading image data and applying it to `Texture2D` objects.

#### Static Methods

-   `static bool LoadToTexture(Nyx::Renderer::GL::Texture2D& texture, const std::string& path, int textureUnit = 0)`
    -   Loads an image from the specified `path` using `stb_image.h` and uploads its data to the provided `Texture2D` object.
    -   `texture`: The `Texture2D` object to load the image data into.
    -   `path`: The file path to the image.
    -   `textureUnit`: The texture unit to bind the texture to (default: `0`).
    -   Returns `true` on successful loading, `false` otherwise.

### `Nyx::InputHandler`

The `Nyx::InputHandler` class manages keyboard and mouse input for a GLFW window.

#### Constructor

```cpp
InputHandler(GLFWwindow* window);
```

-   `window`: A pointer to the GLFW window associated with this input handler.

#### Public Methods

-   `bool isKeyPressed(int key) const`
    -   Checks if a specific keyboard `key` is currently pressed.
    -   `key`: The GLFW key code (e.g., `GLFW_KEY_SPACE`, `GLFW_KEY_W`).
    -   Returns `true` if the key is pressed, `false` otherwise.

-   `bool isMouseButtonPressed(int button) const`
    -   Checks if a specific mouse `button` is currently pressed.
    -   `button`: The GLFW mouse button code (e.g., `GLFW_MOUSE_BUTTON_LEFT`).
    -   Returns `true` if the button is pressed, `false` otherwise.

-   `void getMousePosition(double& x, double& y) const`
    -   Retrieves the current cursor position within the window.
    -   `x`: Reference to a double to store the X-coordinate.
    -   `y`: Reference to a double to store the Y-coordinate.

-   `double getMouseX() const`
    -   Returns the current X-coordinate of the mouse cursor.

-   `double getMouseY() const`
    -   Returns the current Y-coordinate of the mouse cursor.

### `Nyx::Renderer::GL::IBO`

The `Nyx::Renderer::GL::IBO` class represents an OpenGL Index Buffer Object, used to store indices for indexed drawing.

#### Constructor

```cpp
IBO();
```

-   Generates a new OpenGL Index Buffer Object ID.

#### Destructor

```cpp
~IBO();
```

-   Deletes the OpenGL Index Buffer Object.

#### Public Methods

-   `void data(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW)`
    -   Uploads index `data` to the IBO.
    -   `data`: Pointer to the index data.
    -   `size`: Size of the data in bytes.
    -   `usage`: Specifies the expected usage pattern of the data store (e.g., `GL_STATIC_DRAW`, `GL_DYNAMIC_DRAW`).

-   `void bind() const`
    -   Binds the IBO, making it the active index buffer.

-   `void unbind() const`
    -   Unbinds the IBO.

#### Inline Getters

-   `GLuint getID() const`
    -   Returns the OpenGL ID of the IBO.

### `Nyx::Renderer::GL::Renderer`

The `Nyx::Renderer::GL::Renderer` class provides a high-level interface for drawing multiple Vertex Array Objects (VAOs).

#### Constructor

```cpp
Renderer(VAO** vaoList, size_t vaoCount, GLenum drawMode);
```

-   `vaoList`: An array of pointers to `VAO` objects to be rendered.
-   `vaoCount`: The number of `VAO` objects in the `vaoList`.
-   `drawMode`: The OpenGL primitive type to draw (e.g., `GL_TRIANGLES`, `GL_LINES`).

#### Public Methods

-   `void draw(DrawCallback callback = nullptr, void* userData = nullptr)`
    -   Draws all VAOs managed by this renderer.
    -   `callback`: An optional callback function of type `std::function<void(int index, VAO* vao, void* userData, bool& skipDraw)>`.
        -   `index`: The index of the current VAO in the `vaoList`.
        -   `vao`: A pointer to the current `VAO` object being processed.
        -   `userData`: A user-defined pointer passed to the callback.
        -   `skipDraw`: A boolean reference that can be set to `true` within the callback to skip drawing the current VAO.
    -   `userData`: An optional user-defined data pointer that will be passed to the `DrawCallback`.

### `Nyx::Renderer::GL::Shader`

The `Nyx::Renderer::GL::Shader` class handles the compilation, linking, and management of OpenGL shader programs.

#### Constructor

```cpp
Shader(const std::string& vertexPath, const std::string& fragmentPath);
```

-   `vertexPath`: File path to the vertex shader source code.
-   `fragmentPath`: File path to the fragment shader source code.

#### Destructor

```cpp
~Shader();
```

-   Deletes the OpenGL shader program.

#### Public Methods

-   `void bind() const`
    -   Activates the shader program for rendering.

-   `void unbind() const`
    -   Deactivates the current shader program.

-   `void setUniform1i(const std::string& name, int value)`
    -   Sets an integer uniform variable in the shader.

-   `void setUniform1f(const std::string& name, float value)`
    -   Sets a float uniform variable in the shader.

-   `void setUniform2f(const std::string& name, float x, float y)`
    -   Sets a 2-component float vector uniform variable in the shader.

-   `void setUniform3f(const std::string& name, float x, float y, float z)`
    -   Sets a 3-component float vector uniform variable in the shader.

-   `void setUniform4f(const std::string& name, float x, float y, float z, float w)`
    -   Sets a 4-component float vector uniform variable in the shader.

-   `void setUniformMat4fv(const std::string& name, const float* matrix, bool transpose = false)`
    -   Sets a 4x4 matrix uniform variable in the shader.
    -   `matrix`: Pointer to the float array representing the matrix.
    -   `transpose`: If `true`, the matrix will be transposed.

#### Inline Getters

-   `unsigned int getID() const`
    -   Returns the OpenGL ID of the shader program.

### `Nyx::Renderer::GL::Texture2D`

The `Nyx::Renderer::GL::Texture2D` class manages OpenGL 2D textures.

#### `struct TextureParams`

Used to configure texture sampling parameters.

```cpp
struct TextureParams {
    GLint wrapS = GL_REPEAT;          // Texture wrapping mode for S coordinate (default: GL_REPEAT)
    GLint wrapT = GL_REPEAT;          // Texture wrapping mode for T coordinate (default: GL_REPEAT)
    GLint minFilter = GL_LINEAR_MIPMAP_LINEAR; // Minification filter (default: GL_LINEAR_MIPMAP_LINEAR)
    GLint magFilter = GL_LINEAR;      // Magnification filter (default: GL_LINEAR)
};
```

#### Constructor

```cpp
Texture2D();
```

-   Generates a new OpenGL 2D texture ID.

#### Destructor

```cpp
~Texture2D();
```

-   Deletes the OpenGL 2D texture.

#### Public Methods

-   `void setData(int width, int height, int channels, const void* data, const TextureParams& params = {})`
    -   Uploads pixel `data` to the texture. Automatically generates mipmaps.
    -   `width`: Width of the image.
    -   `height`: Height of the image.
    -   `channels`: Number of color channels (e.g., 3 for RGB, 4 for RGBA).
    -   `data`: Pointer to the raw pixel data.
    -   `params`: Optional `TextureParams` to configure wrapping and filtering.

-   `void bind(unsigned int slot = 0) const`
    -   Binds the texture to a specific texture `slot` (e.g., `GL_TEXTURE0 + slot`).

-   `void unbind() const`
    -   Unbinds the currently active texture.

#### Inline Getters

-   `GLuint id() const`
    -   Returns the OpenGL ID of the texture.

### `Nyx::Renderer::GL::VAO`

The `Nyx::Renderer::GL::VAO` class represents an OpenGL Vertex Array Object, which stores the configuration of vertex attributes.

#### `struct VertexAttribute`

Defines the properties of a single vertex attribute.

```cpp
struct VertexAttribute {
    GLuint index;       // Location in shader (e.g., layout (location = 0))
    GLint size;         // Number of components (e.g., 3 for vec3, 2 for vec2)
    GLenum type;        // Data type (e.g., GL_FLOAT, GL_UNSIGNED_INT)
    GLboolean normalized; // Whether integer data should be normalized to float
    GLsizei stride;     // Byte offset between consecutive generic vertex attributes
    size_t offset;      // Byte offset of the first component of this attribute in the buffer
};
```

#### Constructor

```cpp
VAO(const VBO& vbo);
```

-   `vbo`: A `VBO` object that this VAO will manage attributes for.

#### Destructor

```cpp
~VAO();
```

-   Deletes the OpenGL Vertex Array Object.

#### Public Methods

-   `void bind() const`
    -   Binds the VAO, making it the active vertex array object.

-   `void unbind() const`
    -   Unbinds the VAO.

-   `void setLayout(const std::vector<VertexAttribute>& layout)`
    -   Configures the vertex attributes based on the provided `layout`.
    -   `layout`: A vector of `VertexAttribute` structs defining the layout of vertex data in the VBO.

-   `void attachIndexBuffer(const IBO& ibo)`
    -   Attaches an `IBO` to this VAO, associating it with the vertex array state.

#### Inline Getters

-   `inline GLuint getID()`
    -   Returns the OpenGL ID of the VAO.

### `Nyx::Renderer::GL::VBO`

The `Nyx::Renderer::GL::VBO` class represents an OpenGL Vertex Buffer Object, used to store vertex data.

#### Constructor

```cpp
VBO();
```

-   Generates a new OpenGL Vertex Buffer Object ID.

#### Destructor

```cpp
~VBO();
```

-   Deletes the OpenGL Vertex Buffer Object.

#### Public Methods

-   `void data(const void* data, GLsizeiptr size, GLenum usage)`
    -   Uploads vertex `data` to the VBO.
    -   `data`: Pointer to the vertex data.
    -   `size`: Size of the data in bytes.
    -   `usage`: Specifies the expected usage pattern of the data store (e.g., `GL_STATIC_DRAW`, `GL_DYNAMIC_DRAW`).

-   `void bind() const`
    -   Binds the VBO, making it the active vertex buffer.

-   `void unbind() const`
    -   Unbinds the VBO.

#### Inline Getters

-   `inline GLuint getID() const`
    -   Returns the OpenGL ID of the VBO.

## 💡 Real-World Example: Rendering a Textured Rectangle

This example demonstrates how to use Nyx to create a window, set up vertex data, load shaders, apply a texture, and render a simple textured rectangle. It integrates `Window`, `InputHandler`, `VBO`, `IBO`, `VAO`, `Shader`, `Texture2D`, `Image::Loader`, and `Renderer`.

**Prerequisites for this example:**

1.  **GLM**: Ensure GLM is set up in your project for matrix operations.
2.  **Shader Files**: Create `basic.vert` and `basic.frag` in your project directory:

    ```glsl:basic.vert
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
    ```

    ```glsl:basic.frag
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D ourTexture;

    void main()
    {
        FragColor = texture(ourTexture, TexCoord);
    }
    ```

3.  **Texture Image**: Place a `container.jpg` image file in your project directory.

```cpp
#include <glad/glad.h> // Assuming GLAD is used

#include "Nyx/Window.h"
#include "Nyx/Input/InputHandler.h"
#include "Nyx/Renderer/GL/VAO.h"
#include "Nyx/Renderer/GL/VBO.h"
#include "Nyx/Renderer/GL/IBO.h"
#include "Nyx/Renderer/GL/Shader.h"
#include "Nyx/Renderer/GL/Texture2D.h"
#include "Nyx/Renderer/GL/Renderer.h"
#include "Nyx/Image/ImageLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main()
{
    // 1. Window Creation and Setup
    Nyx::Window::WindowConfig config;
    config.glMajorVersion = 3;
    config.glMinorVersion = 3;
    config.coreProfile = true;
    config.debugContext = true;
    config.resizable = true;

    Nyx::Window::Window window("Nyx Textured Rectangle", 800, 600, config);
    gladLoadGL();

    glEnable(GL_DEPTH_TEST);

    // 2. Vertex Data (VBO) and Index Data (IBO) Creation
    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    Nyx::Renderer::GL::VBO vbo;
    vbo.data(vertices, sizeof(vertices), sizeof(GL_FLOAT), GL_STATIC_DRAW);

    Nyx::Renderer::GL::IBO ibo;
    ibo.data(indices, sizeof(indices), sizeof(GL_FLOAT), GL_STATIC_DRAW);

    // 3. VAO Setup with Vertex Attributes
    Nyx::Renderer::GL::VAO vao(&vbo);
    vao.bind();

    vao.setLayout({
        { 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0 }, // Position attribute
        { 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 3 * sizeof(float) }
        });// Texture coordinate attribute);
    vao.attachIndexBuffer(&ibo);

    vao.unbind(); // Unbind VAO after setup
    vbo.unbind(); // Unbind VBO
    ibo.unbind(); // Unbind IBO

    // 4. Shader Loading and Compilation
    Nyx::Renderer::GL::Shader shader("<path_to_vertex_shader_from_your_exectutable>", "<path_to_fragment_shader_from_your_exectutable>");

    // 5. Texture Loading and Binding
    Nyx::Renderer::GL::Texture2D texture;
    Nyx::Image::Loader::LoadToTexture(texture, "<path_to_texture_from_your_exectutable>", 0);

    texture.unbind(); // Unbind texture after loading

    // 6. Renderer Usage to Draw the Scene
    Nyx::Renderer::GL::VAO* vaos[] = { &vao };
    Nyx::Renderer::GL::Renderer renderer(vaos, 1, GL_TRIANGLES);

    // Transformation matrices
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), 45.0f ,glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)window.getWidth() / window.getHeight(), 0.1f, 100.0f);

    // Main application loop
    while (!window.windowClosed())
    {
        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        shader.setUniformMat4fv("model", glm::value_ptr(model));
        shader.setUniformMat4fv("view", glm::value_ptr(view));
        shader.setUniformMat4fv("projection", glm::value_ptr(projection));

        texture.bind(0); // Bind texture to unit 0
        shader.setUniform1i("ourTexture", 0); // Tell shader which texture unit to use

        renderer.draw(); // Draw the VAO
        window.update();
    }

    return 0;
}
```

## 📜 License

Nyx is released under the MIT License.
Includes `stb_image.h` by Sean Barrett (Public Domain / MIT).
