#include "actors/Spectator.h"
#include "core/Scene.h"
#include "core/Input.h"


void Spectator::Start()
{
	
}


void Spectator::Tick(double dT)
{
	m_Yaw -= Input::MouseDelta.x * 0.01f;
	m_Pitch -= Input::MouseDelta.y * 0.01f;

	constexpr float pi = 3.1415192653589793238462643;
	m_Pitch = glm::clamp(m_Pitch, -pi / 2, pi / 2);

	float fwd = (Input::IsKeyDown(Key::W) ? 1 : 0) - (Input::IsKeyDown(Key::S) ? 1 : 0);
	float right = (Input::IsKeyDown(Key::D) ? 1 : 0) - (Input::IsKeyDown(Key::A) ? 1 : 0);

	MatrixTransform& view = GetScene()->CurrentCamera->Transform;
	view.SetRotation(m_Pitch, m_Yaw, 0.0f);

	glm::vec3 move = fwd * view.GetForwardVector() + right * view.GetRightVector();
	view.Position += move * 10.f * (float)dT;

	Actor::Tick(dT);
}