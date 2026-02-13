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


BaseMeshComponent::BaseMeshComponent()
{	
	
}


void BaseMeshComponent::OnSceneChanged(Scene* newScene)
{
	m_RenderState = newScene->PrimitiveRenderStates.Create();
	m_RenderState->Mesh = m_Mesh;
	m_RenderState->DrawMaterial = m_Material;
	m_RenderState->Transform = Transform.GetMatrix();
}


void BaseMeshComponent::Start()
{

}


void BaseMeshComponent::AttachTo(Actor* actor)
{


	ActorComponent::AttachTo(actor);
}