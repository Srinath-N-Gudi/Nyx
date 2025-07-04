# Nyx

**Nyx** is a lightweight C++ wrapper around OpenGL and GLFW that makes it easier to build rendering applications.
It handles the basic setup like creating a window, managing OpenGL context, handling input, loading shaders,
working with textures, and more — so you can focus on rendering.

Perfect for anyone starting with OpenGL or building their own 2D/3D graphics engine.

---

## ✨ Features

- Easy window creation using GLFW
- Input handling built into the window system
- Clean wrappers for VAO, VBO, IBO
- Simple shader management
- Texture support (using `stb_image.h`)
- Flexible and modular code structure

---

## 🚀 Getting Started

*Note*: Only glad, glew can be used and Nyx requires either `<glad/glad.h>` or `<GL/glew.h>` to be included.
To use glad, include `<glad/glad.h>` in your project's include path and define `NYX_USE_GLAD` pre-processor before using `Nyx`.
To use glew, include `<GL/glew.h>` in your project's include path and define `NYX_USE_GLEW` pre-processor before using `Nyx`.




1. **Clone the repo**
   ```bash
   git clone https://github.com/Srinath-N-Gudi/Nyx.git
   ```

2. **Add dependencies**
   - [GLFW](https://www.glfw.org/)
   - [GLAD](https://glad.dav1d.de/) or your own loader (Nyx expects context to be current before GL calls)
   - [GLM](https://github.com/g-truc/glm) (optional, only used in examples)
   - [`stb_image.h`](https://github.com/nothings/stb) for image loading (already included)

3. **Include Nyx in your project**
   ```cpp
   #include "Nyx/Window.h"
   #include "Nyx/Renderer/Shader.h"
   #include "Nyx/Renderer/Texture2D.h"
   ```

4. **Example usage**
   ```cpp
   Nyx::Window window("My App", 1280, 720);

   while (!window.windowClosed())
   {
       glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
       glClear(GL_COLOR_BUFFER_BIT);

       // draw stuff here...

       window.update();
   }
   ```

---

## 📦 Notes

- The window context is automatically made current.
- You are free to use **GLAD**, **GLEW**, or any other OpenGL loader — Nyx doesn’t include one.
- Textures are loaded using `stb_image.h` (MIT/Public Domain).
- GLM is optional and only used in example projects.

---

## 📜 License

Nyx is released under the MIT License.
Includes `stb_image.h` by Sean Barrett (Public Domain / MIT).
