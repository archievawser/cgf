#include "core/Window.h"


bool Window::m_GLFWInitialized;


Window::Window(const char *name, int width, int height)
{
	if(!m_GLFWInitialized)
	{
		glfwInit();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_WindowHandle = glfwCreateWindow(width, height, name, nullptr, nullptr);
}


void Window::Poll()
{
	glfwPollEvents();
}