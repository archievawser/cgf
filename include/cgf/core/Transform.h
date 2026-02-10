#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "core/Common.h"


struct MatrixTransform
{
	MatrixTransform()
	{
		Position = glm::vec3(0);
		Rotation = glm::vec3(0);
		Scale = glm::vec3(1);
	}
	
	FORCEINLINE glm::mat4 GetMatrix() const
	{
		glm::mat4 matrix (1.0f);
		
		matrix = glm::translate(matrix, Position);
		
		matrix *= glm::mat4_cast(Rotation);
		
		matrix = glm::scale(matrix, Scale);
  
		return matrix;
	}
	
	FORCEINLINE glm::mat4 GetViewMatrix() const
	{
		glm::mat4 view (1.0f);

		view = glm::mat4_cast(glm::conjugate(Rotation)) * view;
		
		view = glm::translate(view, -Position);
  
		return view;
	}

	FORCEINLINE void LookAt(glm::vec3 p)
	{
		Rotation = glm::quatLookAt(p, glm::vec3(0, 1, 0));
	}

	FORCEINLINE glm::vec3 GetRightVector() const
	{
		return Rotation * glm::vec3(1, 0, 0);
	}

	FORCEINLINE glm::vec3 GetForwardVector() const
	{
		return Rotation * glm::vec3(0, 0, -1);
	}

	void SetRotation(float pitch, float yaw, float roll)
	{
		Rotation = glm::quat(glm::vec3(pitch, yaw, roll));
	}

	void Rotate(float pitch, float yaw, float roll)
	{
		Rotation *= glm::quat(glm::vec3(pitch, yaw, roll));
	}

	glm::vec3 Position;
	glm::vec3 Scale = glm::vec3(1);
	glm::quat Rotation;
};