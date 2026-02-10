#pragma once

#include "glm/glm.hpp"

#include "core/Actor.h"


class Spectator : public Actor
{
public:
	void Start() override;

	void Tick(double dT) override;

private:
	float m_Yaw = -0.1f;
	float m_Pitch = -0.1f;	
};