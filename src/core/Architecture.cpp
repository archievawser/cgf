#include <algorithm>

#include "core/Architecture.h"


Actor::Actor()
{
	m_ComponentBitset = std::vector<bool>(SceneTypeInfo::TypeCount);
}


void Actor::Start()
{
	OnComponentStart.Invoke();
}


void Actor::Tick(double deltaTime)
{
	OnComponentTick.Invoke(deltaTime);
}


void Actor::AttachTo(Scene* scene)
{
	m_StartListener = scene->OnEntityStart.Connect(this, &Actor::Start);
	m_TickListener = scene->OnEntityTick.Connect(this, &Actor::Tick);
	OnSceneChanged.Invoke(scene);

	m_Scene = scene;
}


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


Scene::Scene()
{
	
}


void PrimitiveComponent::OnSceneChanged(Scene* newScene)
{
	
}


int SceneTypeInfo::TypeCount;