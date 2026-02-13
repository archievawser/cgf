#pragma once

#include <vector>

#include "core/Events.h"
#include "core/Memory.h"


class Scene;


class Actor
{
public:
	Actor();
	
	virtual void Start();

	virtual void Tick(double deltaTime);

	void AttachTo(Scene* scene);

	template<typename ComponentT>
	void AddComponent(SharedPtr<ComponentT> component)
	{
		RegisterComponent(component);
		component->AttachTo(this);
	}

	template<typename ComponentT>
	void RegisterComponent(SharedPtr<ComponentT> component)
	{
		GetComponentRegistry<ComponentT>().push_back(component);
	}
	
	template<typename ComponentT>
	std::vector<SharedPtr<ComponentT>>& GetComponentRegistry()
	{
		static std::vector<SharedPtr<ComponentT>> components;

		return components;
	}

	FORCEINLINE Scene* GetScene() const
	{
		return m_Scene;
	}

	OnStartEvent OnComponentStart;
	OnTickEvent OnComponentTick;
	Event<Scene*> OnSceneChanged;

private:
	bool m_ShouldTick = true;
	Scene* m_Scene = nullptr;
	std::vector<bool> m_ComponentBitset;
	OnTickEvent::Connection m_TickListener;
	OnStartEvent::Connection m_StartListener;
};