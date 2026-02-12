#include "components/DynamicMesh.h"


DynamicMeshComponent::DynamicMeshComponent()
{
	m_Mesh = SharedPtr<DynamicMesh>::Create();
	m_DrawInfo = SharedPtr<PrimitiveRenderState>::Create();
	m_DrawInfo->Mesh = m_Mesh;
	m_DrawInfo->DrawMaterial = m_Material;
	m_DrawInfo->Transform = Transform.GetMatrix();
}


void DynamicMeshComponent::SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices)
{

}


void DynamicMeshComponent::SetIndexData(unsigned int* indices, unsigned int numIndices)
{

}