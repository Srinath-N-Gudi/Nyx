#include "Window.h"

namespace Nyx  {
namespace Window  {
		
		Window::Window(const char* windowTitle, int width, int height, const WindowConfig& config)
			:m_Width(width), m_Height(height),m_WindowTitle(windowTitle),
			 m_WindowObject(nullptr), m_InputHandler(nullptr), m_WConfig(config)
		{
			this->init();
		}

		Window::~Window()
		{
			glfwDestroyWindow(m_WindowObject);
			glfwTerminate();
		}

		void Window::init()
		{
			if (!glfwInit()) {
				std::cerr << "Failed to initialize GLFW!" << std::endl;
				return;
			}
			// GL Config should be set only once before creating the window object
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_WConfig.glMajorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_WConfig.glMinorVersion);

			glfwWindowHint(GLFW_OPENGL_PROFILE,
				m_WConfig.coreProfile ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);

			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, m_WConfig.debugContext ? GLFW_TRUE : GLFW_FALSE);
			glfwWindowHint(GLFW_RESIZABLE, m_WConfig.resizable ? GLFW_TRUE : GLFW_FALSE);

			GLFWwindow* window = glfwCreateWindow(m_Width, m_Height, m_WindowTitle, NULL, NULL);
			m_WindowObject = window;
			m_InputHandler = Nyx::InputHandler(m_WindowObject);
			if (!window) {
				std::cerr << "Failed to create GLFW window!" << std::endl;
				glfwTerminate();
			}
			glfwMakeContextCurrent(window);
	
		}

		void Window::update()
		{

			glfwSwapBuffers(m_WindowObject);
			glfwPollEvents();
		}



		// Setters
		void Window::setResizeCallback(const NyxResizeCallback& callback) {
			m_ResizeCallback = callback;
			glfwSetWindowSizeCallback(m_WindowObject, Window::glfwResizeCallback);
			glfwSetWindowUserPointer(m_WindowObject, this); // Required for callback context
		}
		void Window::setCursorPosCallback(const NyxCursorPosCallback& callback)
		{
			m_CursorPosCallback = callback;
			glfwSetCursorPosCallback(m_WindowObject, Window::glfwCursorPosCallback);
			glfwSetWindowUserPointer(m_WindowObject, this); // Required for callback context
		}
		void Window::glfwResizeCallback(GLFWwindow* window, int width, int height) {
			auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (self) {
				self->m_Width = width;
				self->m_Height = height;

				if (self->m_ResizeCallback) {
					self->m_ResizeCallback(width, height);
				}
			}
		}
		void Window::glfwCursorPosCallback(GLFWwindow* window, double x, double y) {
			auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (self) {
				if (self->m_CursorPosCallback) {
					self->m_CursorPosCallback(x, y);
				}
			}
		}
		void Window::setWindowTitle(const char* windowTitle)
		{
			m_WindowTitle = windowTitle;
			glfwSetWindowTitle(m_WindowObject, m_WindowTitle);

		}
		void Window::setHeight(const int& height)
		{
			m_Height = height;
			glfwSetWindowSize(m_WindowObject, m_Width, m_Height);
		}
		void Window::setWidth(const int& width)
		{
			m_Width = width;
			glfwSetWindowSize(m_WindowObject, m_Width, m_Height);
		}
		void Window::setSize(const int& width, const int& height)
		{
			m_Width = width;
			m_Height = height;
			glfwSetWindowSize(m_WindowObject, m_Width, m_Height);
		}

		void Window::maximize() {
			glfwMaximizeWindow(m_WindowObject);
		}
		void Window::minimize() {
			glfwIconifyWindow(m_WindowObject); 
		}
		void Window::restore() {
			glfwRestoreWindow(m_WindowObject);
		}

		void Window::getFramebufferSize(int& width, int& height) const {
			width = m_Width;
			height = m_Height;
		}
		Nyx::InputHandler& Window::getInputHandler() {
			return m_InputHandler;
		}
		Nyx::InputHandler* Window::operator->() {
			return &m_InputHandler;
		}

	}
}
