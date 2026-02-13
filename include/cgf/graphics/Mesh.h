#pragma once

#include "graphics/Diligent.h"

#include "core/Common.h"


class BaseMesh
{
public:
	BaseMesh();

	BaseMesh(unsigned int indexCount, 
		RefCntAutoPtr<IBuffer> indexBuffer, 
		RefCntAutoPtr<IBuffer> vertexBuffer);

	FORCEINLINE unsigned int GetIndexCount() const
	{
		return m_IndexCount;
	}

	FORCEINLINE RefCntAutoPtr<IBuffer> GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	FORCEINLINE RefCntAutoPtr<IBuffer> GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

	FORCEINLINE bool IsValid() const
	{
		return m_IndexBuffer && m_VertexBuffer;
	}

protected:
	unsigned int m_IndexCount = 0;
	RefCntAutoPtr<IBuffer> m_IndexBuffer;
	RefCntAutoPtr<IBuffer> m_VertexBuffer;
};


class StaticMesh : public BaseMesh
{
public:
	StaticMesh(unsigned int indexCount, 
		RefCntAutoPtr<IBuffer> indexBuffer, 
		RefCntAutoPtr<IBuffer> vertexBuffer);
};


class DynamicMesh : public BaseMesh
{
public:
	DynamicMesh() = default;

	void SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices);

	void SetIndexData(unsigned int* indices, unsigned int numIndices);

private:
	int m_CurrentVertexBufferSize = 0;
	int m_CurrentIndexBufferSize = 0;
};
