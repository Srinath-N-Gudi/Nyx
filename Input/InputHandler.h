#pragma once
#include <GLFW/glfw3.h>

namespace Nyx {

    class InputHandler {
    public:
        InputHandler(GLFWwindow* window);

        bool isKeyPressed(int key) const;
        bool isMouseButtonPressed(int button) const;

        void getMousePosition(double& x, double& y) const;
        double getMouseX() const;
        double getMouseY() const;

    private:
        GLFWwindow* m_Window;
    };

} // namespace Nyx
