#include "core/Input.h"
#include "core/Game.h"
#include "core/Events.h"
#include "GLFW/glfw3.h"


Input::Input(Window* window)
	: m_Window(window)
{
	MouseDelta = glm::vec2(0);

	double x, y;
	glfwGetCursorPos(m_Window->GetWindowHandle(), &x, &y);
	MousePosition = glm::vec2(x, y);

	glfwSetKeyCallback(window->GetWindowHandle(), &Input::OnKeyPressedInternal);
}


void Input::OnKeyPressedInternal(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		Input::m_PressedKeys[key] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		Input::m_PressedKeys[key] = false;
	}

	OnKeyPressed.Invoke((Key)key);
}


void Input::SetCursorState(bool disabled)
{
	glfwSetInputMode(m_Window->GetWindowHandle(), GLFW_CURSOR, disabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}


void Input::NewInputFrame()
{
	double x, y;
	glfwGetCursorPos(m_Window->GetWindowHandle(), &x, &y);

	glm::vec2 newMousePosition = glm::vec2(x, y);

	MouseDelta = newMousePosition - MousePosition;
	MousePosition = newMousePosition;
}


bool Input::m_PressedKeys[(int)Key::MAX_KEY_VALUE];


Event<Key> Input::OnKeyPressed;


glm::vec2 Input::MousePosition;


glm::vec2 Input::MouseDelta;