#pragma once

#include "core/Actor.h"
#include "core/Component.h"


class SpriteBatch : public Actor
{
public:
	

private:
	SharedPtr<PrimitiveRenderState> m_DrawInfo;
};