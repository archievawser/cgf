#pragma once

#include <vector>

#include "core/Window.h"
#include "core/Events.h"

#include "glm/glm.hpp"


// This could be done in a more stylish manner,
// but that would require making assumptions about
// the GLFW enums which may be subject to change.
enum class Key
{
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,

	LeftShift = GLFW_KEY_LEFT_SHIFT,
	RightShift = GLFW_KEY_RIGHT_SHIFT,

	MAX_KEY_VALUE
};


class Input
{
public:
	Input(Window* window);

	static FORCEINLINE bool IsKeyDown(Key key)
	{
		return m_PressedKeys[(int)key];
	}

	void SetCursorState(bool disabled);

	void NewInputFrame();

	static Event<Key> OnKeyPressed;
	static glm::vec2 MouseDelta;
	static glm::vec2 MousePosition;

private:
	static void OnKeyPressedInternal(GLFWwindow* window, int key, int scancode, int action, int mods);

	Window* m_Window;
	static bool m_PressedKeys[(int)Key::MAX_KEY_VALUE];
};