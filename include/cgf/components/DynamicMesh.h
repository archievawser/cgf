#pragma once

#include "core/Actor.h"
#include "core/Component.h"
#include "core/Memory.h"

#include "graphics/Renderer.h"


class DynamicMeshComponent : public SceneComponent
{
public:
	DynamicMeshComponent();

	void SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices);

	void SetIndexData(unsigned int* indices, unsigned int numIndices);

private:
	SharedPtr<MaterialInstance> m_Material;
	SharedPtr<DynamicMesh> m_Mesh;
	SharedPtr<PrimitiveRenderState> m_DrawInfo;
};