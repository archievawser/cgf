#include "core/Actor.h"
#include "core/Scene.h"


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
	m_Scene = scene;
	
	if(scene == nullptr)
	{
		m_StartListener = nullptr;
		m_TickListener = nullptr;

		return;
	}

	OnSceneChanged.Invoke(scene);

	m_StartListener = scene->OnStartActors.Connect(this, &Actor::Start);

	if(m_ShouldTick)
	{
		m_TickListener = scene->OnTickActors.Connect(this, &Actor::Tick);
	}
}
