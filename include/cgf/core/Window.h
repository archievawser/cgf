#pragma once

#if PLATFORM_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#if PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11 1
#endif

#if PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#include "glfw/glfw3.h"
#include "GLFW/glfw3native.h"

#include "core/Common.h"
#include "graphics/Diligent.h"


class Window
{
public:
	Window(const char* name, int width, int height);

	void Poll();

	FORCEINLINE NativeWindow GetNativeWindowHandle() const
	{
#if PLATFORM_WIN32
		Win32NativeWindow Window { glfwGetWin32Window(m_WindowHandle) };
#endif

#if PLATFORM_LINUX
		LinuxNativeWindow Window;
		Window.WindowId = glfwGetX11Window(m_WindowHandle);
		Window.pDisplay = glfwGetX11Display();
#endif

#if PLATFORM_MACOS
		MacOSNativeWindow Window { GetNSWindowView(m_WindowHandle) };
#endif

		return Window;
	}

	FORCEINLINE GLFWwindow* GetWindowHandle() const
	{
		return m_WindowHandle;
	}

	FORCEINLINE bool ShouldClose() const
	{
		return glfwWindowShouldClose(m_WindowHandle);
	}

private:
	static bool m_GLFWInitialized;
	GLFWwindow* m_WindowHandle;
};