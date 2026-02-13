#pragma once

#include "core/Memory.h"
#include "core/Actor.h"
#include "core/Events.h"
#include "core/Transform.h"
#include "core/AssetLibrary.h"

#include "graphics/Renderer.h"
#include "graphics/Mesh.h"


class Scene;


class ActorComponent
{
public:
	ActorComponent();

	virtual void Start();

	virtual void TickComponent(double deltaTime);

	virtual void OnSceneChanged(Scene* newScene);

	virtual void AttachTo(Actor* entity);

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
	MatrixTransform Transform;
};


class BaseMeshComponent : public SceneComponent
{
public:
	BaseMeshComponent();

	void OnSceneChanged(Scene* newScene) override;

	void Start() override;

	void AttachTo(Actor* entity) override;

	void TickComponent(double dT) override
	{
		if(m_RenderState)
		{
			m_RenderState->Transform = Transform.GetMatrix();
		}
	}

	FORCEINLINE SharedPtr<BaseMesh> GetMesh() const
	{
		return m_Mesh;
	}
	
	FORCEINLINE void SetMesh(SharedPtr<BaseMesh> mesh)
	{
		m_Mesh = mesh;
		
		if(m_RenderState)
		{
			m_RenderState->Mesh = mesh;
		}
	}

	FORCEINLINE SharedPtr<MaterialInstance> GetMaterial() const
	{
		return m_Material;
	}

	FORCEINLINE void SetMaterial(SharedPtr<MaterialInstance> material)
	{
		m_Material = material;
		
		if(m_RenderState)
		{
			m_RenderState->DrawMaterial = material;
		}
	}

private:
	bool m_StateInvalid = false;
	PooledPtr<PrimitiveRenderState> m_RenderState;
	SharedPtr<BaseMesh> m_Mesh;
	SharedPtr<MaterialInstance> m_Material;
};