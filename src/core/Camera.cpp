#include "core/Camera.h"


glm::mat4 Camera::GetOrthographicProjection(float width, float height)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight);
}


glm::mat4 Camera::GetPerspectiveProjection(float fov, float width, float height)
{
	return glm::perspectiveFov(fov, width, height, 0.1f, 1000.f);
}