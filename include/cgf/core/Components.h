#pragma once

#include "core/Memory.h"
#include "core/Actor.h"
#include "core/Events.h"
#include "core/Transform.h"
#include "core/AssetLibrary.h"


class Scene;


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
	MatrixTransform Transform;
};


struct MeshDrawInfo
{
	glm::mat4 Transform;
	SharedPtr<StaticMesh> Mesh;
	SharedPtr<MaterialInstance> DrawMaterial;
};


class MeshComponent : public SceneComponent
{
public:
	MeshComponent();

	void OnSceneChanged(Scene* newScene) override;

	void Start() override
	{
		SubmitDrawInfo();
	}

	void TickComponent(double dT) override
	{
		m_DrawInfo->Transform = Transform.GetMatrix();
	}

	void SubmitDrawInfo();

	FORCEINLINE SharedPtr<StaticMesh> GetMesh() const
	{
		return m_Mesh;
	}
	
	FORCEINLINE void SetMesh(SharedPtr<StaticMesh> mesh)
	{
		m_Mesh = mesh;
		m_DrawInfo->Mesh = mesh;
	}

	FORCEINLINE SharedPtr<MaterialInstance> GetMaterial() const
	{
		return m_Material;
	}

	FORCEINLINE void SetMaterial(SharedPtr<MaterialInstance> material)
	{
		m_Material = material;
		m_DrawInfo->DrawMaterial = material;
	}

private:
	SharedPtr<MeshDrawInfo> m_DrawInfo;
	SharedPtr<StaticMesh> m_Mesh;
	SharedPtr<MaterialInstance> m_Material;
};