#include "InputHandler.h"

namespace Nyx {

    InputHandler::InputHandler(GLFWwindow* window)
        : m_Window(window)
    {
    }

    bool InputHandler::isKeyPressed(int key) const {
        return glfwGetKey(m_Window, key) == GLFW_PRESS;
    }

    bool InputHandler::isMouseButtonPressed(int button) const {
        return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
    }

    void InputHandler::getMousePosition(double& x, double& y) const {
        glfwGetCursorPos(m_Window, &x, &y);
    }

    double InputHandler::getMouseX() const {
        double x, y;
        getMousePosition(x, y);
        return x;
    }

    double InputHandler::getMouseY() const {
        double x, y;
        getMousePosition(x, y);
        return y;
    }

} // namespace Nyx
