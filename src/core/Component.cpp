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
	
}


void MeshComponent::OnSceneChanged(Scene* newScene)
{

}


void MeshComponent::Start()
{
	m_RenderState = GetScene()->PrimitiveRenderStates.Create();
	m_RenderState->Get().Mesh = m_Mesh;
	m_RenderState->Get().DrawMaterial = m_Material;
	m_RenderState->Get().Transform = Transform.GetMatrix();
}