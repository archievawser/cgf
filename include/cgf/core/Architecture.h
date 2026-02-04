#pragma once

#include <vector>

#include "core/Events.h"


class Scene;


class Actor
{
public:
	Actor();
	
	virtual void Start();

	virtual void Tick(double deltaTime);

	void BindEvents(SharedPtr<Scene> scene);

	template<typename ComponentT>
	void AddComponent(SharedPtr<ComponentT> component)
	{
		component->BindEvents(this);
		
		GetComponentsOfType<ComponentT>.push_back(component);
	}
	
	template<typename ComponentT>
	std::vector<SharedPtr<ComponentT>>& GetComponentsOfType()
	{
		static std::vector<SharedPtr<ComponentT>> components;

		return components;
	}

	OnStartEvent OnComponentStart;
	OnUpdateEvent OnComponentTick;

private:
	OnStartEvent::ConnectionT m_StartListener;
	OnUpdateEvent::ConnectionT m_TickListener;
};


class ActorComponent
{
public:
	ActorComponent();

	virtual void Start();

	virtual void TickComponent(double deltaTime);

	void BindEvents(Actor* entity);

	Actor* Owner;

private:
	OnStartEvent::ConnectionT m_StartListener;
	OnUpdateEvent::ConnectionT m_TickListener;
};


class Scene
{
public:
	Scene();

	template<typename ActorT>
	void AddActor(SharedPtr<ActorT> actor)
	{
		actor->BindEvents(this);
		
		GetActorsOfType<ActorT>.push_back(actor);
	}
	
	template<typename ActorT>
	std::vector<SharedPtr<ActorT>>& GetActorsOfType()
	{
		static std::vector<SharedPtr<ActorT>> actors;

		return actors;
	}

	OnStartEvent OnEntityStart;
	OnUpdateEvent OnEntityTick;
};