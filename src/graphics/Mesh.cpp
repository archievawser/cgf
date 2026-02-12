#include "graphics/Mesh.h"

#include "core/Game.h"


BaseMesh::BaseMesh()
	: m_IndexCount(0), m_IndexBuffer(nullptr), m_VertexBuffer(nullptr)
{

}


BaseMesh::BaseMesh(unsigned int indexCount, RefCntAutoPtr<IBuffer> indexBuffer, RefCntAutoPtr<IBuffer> vertexBuffer)
	: m_IndexCount(indexCount), m_IndexBuffer(indexBuffer), m_VertexBuffer(vertexBuffer)
{

}


StaticMesh::StaticMesh(unsigned int indexCount, RefCntAutoPtr<IBuffer> indexBuffer, RefCntAutoPtr<IBuffer> vertexBuffer)
	: BaseMesh(indexCount, indexBuffer, vertexBuffer)
{

}


void DynamicMesh::SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices)
{
	const int newVertexBufferSize = vertexByteSize * numVertices;

	if (newVertexBufferSize == m_CurrentVertexBufferSize)
	{
		Game->GetGraphicsContext()->GetDeviceContext()->UpdateBuffer(m_VertexBuffer, 
			0, 
			newVertexBufferSize, 
			vertexData,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		return;
	}

	BufferData vertexBufferData (vertexData, newVertexBufferSize);
	BufferDesc vertexBufferDesc;
	vertexBufferDesc.Name = "Vertex Buffer";
	vertexBufferDesc.Size = newVertexBufferSize;
	vertexBufferDesc.Usage = USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;

	Game->GetGraphicsContext()->GetRenderDevice()->CreateBuffer(vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);
	m_CurrentVertexBufferSize = vertexByteSize * numVertices;
}


void DynamicMesh::SetIndexData(unsigned int* indices, unsigned int numIndices)
{
	const int newIndexBufferSize = sizeof(unsigned int) * numIndices;

	if (newIndexBufferSize == m_CurrentIndexBufferSize)
	{
		Game->GetGraphicsContext()->GetDeviceContext()->UpdateBuffer(m_IndexBuffer, 
			0, 
			newIndexBufferSize, 
			indices,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		return;
	}

	BufferData idata(indices, newIndexBufferSize);

	BufferDesc ibufferDesc;
	ibufferDesc.Name = "Index Buffer";
	ibufferDesc.Size = newIndexBufferSize;
	ibufferDesc.Usage = USAGE_DYNAMIC;
	ibufferDesc.BindFlags = BIND_INDEX_BUFFER;

	Game->GetGraphicsContext()->GetRenderDevice()->CreateBuffer(ibufferDesc, &idata, &m_IndexBuffer);
	m_CurrentIndexBufferSize = newIndexBufferSize;
}