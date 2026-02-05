#include "core/Architecture.h"


Actor::Actor()
{

}


void Actor::Start()
{
	CGF_INFO("Started");
	OnComponentStart.Invoke();
}


void Actor::Tick(double deltaTime)
{
	CGF_INFO("Ticked");
	OnComponentTick.Invoke(deltaTime);
}


void Actor::BindEventListeners(SharedPtr<Scene> scene)
{
	CGF_INFO("Binding actor listeners");
	
	m_StartListener = scene->OnEntityStart.Connect(this, &Actor::Start);

	m_TickListener = scene->OnEntityTick.Connect([](double) {
		CGF_INFO("HI");
	});

	CGF_INFO(scene->OnEntityTick.GetNumberOfListeners());
	CGF_INFO("Actor listeners bound");
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