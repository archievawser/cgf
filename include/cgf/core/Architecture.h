#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "core/Events.h"
#include "core/Memory.h"

#include "graphics/Diligent.h"
#include "graphics/Material.h"


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


class Scene;
class MaterialInstance;


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
	Scene* m_Scene = nullptr;
	std::vector<bool> m_ComponentBitset;
	OnTickEvent::Connection m_TickListener;
	OnStartEvent::Connection m_StartListener;
};


class ActorComponent
{
public:
	ActorComponent();

	virtual void Start();

	virtual void TickComponent(double deltaTime);

	virtual void OnSceneChanged(Scene* newScene);

	void AttachTo(Actor* entity);

	FORCEINLINE Scene* GetScene() const
	{
		return Owner->GetScene();
	}

	Actor* Owner;

private:
	OnStartEvent::Connection m_StartListener;
	OnTickEvent::Connection m_TickListener;
	Event<Scene*>::Connection m_SceneChangeListener;
};


class SceneComponent : public ActorComponent
{
public:
	FORCEINLINE glm::mat4 ToMatrix() const
	{
		glm::mat4 matrix (1.0f);
		matrix = glm::translate(matrix, Position);
		matrix = glm::rotate(matrix, 1.0f, Rotation);
		matrix = glm::scale(matrix, Scale);

		return matrix;
	}

	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
};


struct MeshDrawInfo
{
	int IndexCount;
	RefCntAutoPtr<IBuffer> IndexBuffer;
	RefCntAutoPtr<IBuffer> VertexBuffer;
	SharedPtr<MaterialInstance> DrawMaterial;
};


class Scene
{
public:
	Scene();

	template<typename ActorT>
	void AddActor(SharedPtr<ActorT> actor)
	{
		static_assert(std::is_base_of_v<Actor, Actor>,
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

	Event<> OnEntityStart;
	Event<double> OnEntityTick;
	std::vector<MeshDrawInfo> MeshDrawList;

private:
	bool m_InPlay = false;
};


class PrimitiveComponent : public SceneComponent
{
public:
	PrimitiveComponent() = default;

	void OnSceneChanged(Scene* newScene) override;

	void Start() override
	{
		CGF_INFO("Start called");
		GetScene()->MeshDrawList.push_back(PrimitiveInfo);
	}

public:
	MeshDrawInfo PrimitiveInfo;
};