#include "core/Architecture.h"


Actor::Actor()
{

}


void Actor::Start()
{
	OnComponentStart.Invoke();
}


void Actor::Tick(double deltaTime)
{
	OnComponentTick.Invoke(deltaTime);
}


void Actor::BindEvents(SharedPtr<Scene> scene)
{
	m_StartListener = scene->OnEntityStart.Connect(this, &Actor::Start);
	m_TickListener = scene->OnEntityTick.Connect(this, &Actor::Tick);
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


void ActorComponent::BindEvents(Actor* entity)
{
	m_StartListener = entity->OnComponentStart.Connect(this, &ActorComponent::Start);
	m_TickListener = entity->OnComponentTick.Connect(this, &ActorComponent::TickComponent);
}


Scene::Scene()
{
	
}