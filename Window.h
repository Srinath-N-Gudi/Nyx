#pragma once
/**
 * @brief Creates and manages a GLFW window with an active OpenGL context.
 *
 * After construction, the OpenGL context is current. You must load OpenGL
 * functions (e.g., using GLAD or GLEW) yourself.
 *
 * Example:
 *     Nyx::Window::Window window("Game", 1280, 720, config);
 *     gladLoadGL(); // Required! ( if glad is used ) 
 */


#include <iostream>
#include <GLFW/glfw3.h>
#include <functional>
#include "NyxAPI.h"
#include "Input/InputHandler.h"

namespace Nyx
{
	namespace Window
	{
		struct NYX_API WindowConfig {
			int glMajorVersion = 3;
			int glMinorVersion = 3;
			bool coreProfile = true;
			bool debugContext = false;
			bool resizable = true;
		};


		using NyxResizeCallback = std::function<void(int width, int height)>;
		using NyxCursorPosCallback = std::function<void(double x, double y)>;

		class NYX_API Window
		{
		private:
			int m_Width, m_Height;
			const char* m_WindowTitle;
			WindowConfig m_WConfig;
			GLFWwindow* m_WindowObject;
			Nyx::InputHandler m_InputHandler;
			NyxResizeCallback m_ResizeCallback;
			NyxCursorPosCallback m_CursorPosCallback;
		private:
			static void glfwResizeCallback(GLFWwindow* window, int width, int height);
			static void glfwCursorPosCallback(GLFWwindow* window, double x, double y);

			void init();

		public:
			// Inlines
			inline int			getWidth()			const		{ return m_Width; }
			inline int			getHeight()			const		{ return m_Height; }
			inline const char*	getWindowTitle()	const	{ return m_WindowTitle; }
			inline GLFWwindow*	getGLFWWindow()		const	{ return m_WindowObject; }
			
			
			inline bool windowClosed() const { return glfwWindowShouldClose(m_WindowObject) == 1 ; }
			inline bool isMinimized() const { return glfwGetWindowAttrib(m_WindowObject, GLFW_ICONIFIED); }
			inline bool isMaximized() const { return glfwGetWindowAttrib(m_WindowObject, GLFW_MAXIMIZED); }
			inline void setCursorPos(double x, double y) { glfwSetCursorPos(m_WindowObject, x, y); }

			


		public:
			Window(const char* windowTitle, int width, int height, const WindowConfig& config);
			~Window();

			void update();


			void setWindowTitle(const char* windowTile);
			void setWidth (const int& width);
			void setHeight(const int& height);
			void setSize(const int& width, const int& height);
			void setResizeCallback(const NyxResizeCallback& callback);
			void setCursorPosCallback(const NyxCursorPosCallback& callback);
			void getFramebufferSize(int& width, int& height) const;
			Nyx::InputHandler& getInputHandler();

			void maximize();
			void minimize();
			void restore();

			Nyx::InputHandler* operator->();


		

		};


		
	
	}
}