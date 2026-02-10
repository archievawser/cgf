#pragma once

#include "glm/glm.hpp"

#include "core/Transform.h"
#include "core/Actor.h"


class Camera : public Actor
{
public:
	Camera(bool orthographic, float fov, float width, float height)
	{
		Projection = orthographic ? GetOrthographicProjection(width, height)
			: GetPerspectiveProjection(fov, width, height);

		Transform.Position = glm::vec3(0, 0, -10);
	}
	
	MatrixTransform Transform;
	glm::mat4 Projection;

private:
	glm::mat4 GetOrthographicProjection(float width, float height);

	glm::mat4 GetPerspectiveProjection(float fov, float width, float height);
};