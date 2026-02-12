#pragma once

#include <vector>

#include "core/Memory.h"
#include "core/Events.h"
#include "core/Actor.h"
#include "core/Component.h"
#include "core/Camera.h"


struct SceneTypeInfo
{
	static int TypeCount;

	static int NextTypeIndex()
	{
		static int value = 0;
		return value++;
	}

	template<typename T>
	static int GetTypeIndex()
	{
		static int i = (TypeCount++, NextTypeIndex());

		return i;
	}

	template<typename... T>
	static int GetTypeHash()
	{
		int out = 0;
		
		((out |= 1 << GetTypeIndex<T>()), ...);

		return out;
	}
};


class Scene
{
public:
	Scene();

	void Start();

	void Tick(double dT);

	template<typename ActorT>
	void AddActor(SharedPtr<ActorT> actor)
	{
		static_assert(std::is_base_of_v<Actor, ActorT>,
			"ActorT must publicly derive Actor");

		if(m_InPlay)
		{
			actor->Start();
		}

		actor->AttachTo(this);
		
		GetActorsOfType<ActorT>().push_back(actor);
	}
	
	template<typename ActorT>
	std::vector<SharedPtr<ActorT>>& GetActorsOfType()
	{
		static std::vector<SharedPtr<ActorT>> actors;

		return actors;
	}
	
	template<typename ActorT>
	SharedPtr<ActorT> GetFirst()
	{
		return GetActorsOfType<ActorT>()[0];
	}

	Event<> OnStartActors;
	Event<double> OnTickActors;
	Pool<PrimitiveRenderState> PrimitiveRenderStates;
	SharedPtr<Camera> CurrentCamera;

private:
	bool m_InPlay = false;
};