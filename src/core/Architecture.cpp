#include "core/Architecture.h"


Actor::Actor()
{

}


void Actor::Start()
{
	CGF_INFO("Started");
	//OnComponentStart.Invoke();
}


void Actor::Tick(double deltaTime)
{
	CGF_INFO("Ticked");
	//OnComponentTick.Invoke(deltaTime);
}


void Actor::BindEventListeners(Scene* scene)
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


void ActorComponent::BindEventListeners(Actor* entity)
{
	m_StartListener = entity->OnComponentStart.Connect(this, &ActorComponent::Start);
	m_TickListener = entity->OnComponentTick.Connect(this, &ActorComponent::TickComponent);
}


Scene::Scene()
{
	
}