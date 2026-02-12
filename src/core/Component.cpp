#include "core/Component.h"
#include "core/Scene.h"


ActorComponent::ActorComponent()
{

}


void ActorComponent::Start()
{

}


void ActorComponent::TickComponent(double deltaTime)
{

}


void ActorComponent::OnSceneChanged(Scene* newScene)
{
	
}


void ActorComponent::AttachTo(Actor* actor)
{
	Owner = actor;
	m_StartListener = actor->OnComponentStart.Connect(this, &ActorComponent::Start);
	m_TickListener = actor->OnComponentTick.Connect(this, &ActorComponent::TickComponent);
	m_SceneChangeListener = actor->OnSceneChanged.Connect(this, &ActorComponent::OnSceneChanged);
}


MeshComponent::MeshComponent()
{	
	m_DrawInfo = SharedPtr<MeshDrawInfo>::Create();
	m_DrawInfo->Mesh = m_Mesh;
	m_DrawInfo->DrawMaterial = m_Material;
	m_DrawInfo->Transform = Transform.GetMatrix();
}


void MeshComponent::OnSceneChanged(Scene* newScene)
{
	
}


void MeshComponent::SubmitDrawInfo()
{
	GetScene()->MeshDrawList.push_back(m_DrawInfo);
}