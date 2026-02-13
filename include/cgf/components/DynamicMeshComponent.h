#pragma once

#include "core/Actor.h"
#include "core/Component.h"
#include "core/Memory.h"

#include "graphics/Renderer.h"


class DynamicMeshComponent : public BaseMeshComponent
{
public:
	DynamicMeshComponent();

	void SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices);

	void SetIndexData(unsigned int* indices, unsigned int numIndices);

	SharedPtr<DynamicMesh> GetMesh() const
	{
		return (SharedPtr<DynamicMesh>)(BaseMeshComponent::GetMesh());
	}
};