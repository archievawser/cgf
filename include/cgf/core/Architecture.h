#pragma once

#include <vector>

#include "core/Events.h"


class Scene;


class Actor
{
public:
	Actor();

	~Actor()
	{
		
	}
	
	virtual void Start();

	virtual void Tick(double deltaTime);

	void BindEventListeners(SharedPtr<Scene> scene);

	void PollComponentRenderProxies();

	template<typename ComponentT>
	void AddComponent(SharedPtr<ComponentT> component)
	{
		component->BindEventListeners(this);
		
	}

	template<typename ComponentT>
	void RegisterComponent(SharedPtr<ComponentT> component)
	{
		GetComponentRegistry<ComponentT>().push_back(component);
		
		if constexpr (!std::is_same_v<ComponentT::Super, void>)
		{
			RegisterObject<ComponentT::Super>(component);
		}
	}
	
	template<typename ComponentT>
	std::vector<SharedPtr<ComponentT>>& GetComponentRegistry()
	{
		static std::vector<SharedPtr<ComponentT>> components;

		return components;
	}

	OnStartEvent OnComponentStart;
	OnTickEvent OnComponentTick;

private:
	OnStartEvent::Connection m_StartListener;
	OnTickEvent::Connection m_TickListener;
};


class ActorComponent
{
public:
	ActorComponent();

	virtual void Start();

	virtual void TickComponent(double deltaTime);

	void BindEventListeners(Actor* entity);

	Actor* Owner;

private:
	OnStartEvent::Connection m_StartListener;
	OnTickEvent::Connection m_TickListener;
};


class Scene
{
public:
	Scene();

	template<typename ActorT>
	void AddActor(SharedPtr<ActorT> actor)
	{
		CGF_INFO("Adding actor");

		actor->BindEventListeners(this);
		CGF_INFO(OnEntityTick.GetNumberOfListeners());
		
		GetActorsOfType<ActorT>().push_back(actor);
	}
	
	template<typename ActorT>
	std::vector<SharedPtr<ActorT>>& GetActorsOfType()
	{
		static std::vector<SharedPtr<ActorT>> actors;

		CGF_INFO("Registered actor");

		return actors;
	}

	OnStartEvent OnEntityStart;
	OnTickEvent OnEntityTick;
};