#include "components/DynamicMeshComponent.h"


DynamicMeshComponent::DynamicMeshComponent()
{
	SetMesh(SharedPtr<DynamicMesh>::Create());
}


void DynamicMeshComponent::SetVertexData(void* vertexData, unsigned int vertexByteSize, unsigned int numVertices)
{
	((SharedPtr<DynamicMesh>)GetMesh())->SetVertexData(vertexData, vertexByteSize, numVertices);
}


void DynamicMeshComponent::SetIndexData(unsigned int* indices, unsigned int numIndices)
{
	((SharedPtr<DynamicMesh>)GetMesh())->SetIndexData(indices, numIndices);
}