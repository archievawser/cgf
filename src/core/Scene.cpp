#include "core/Scene.h"


Scene::Scene()
{
	CurrentCamera = SharedPtr<Camera>::Create(false, 70.f, 1920.f, 1080.f);
	AddActor(CurrentCamera);
}


void Scene::Start()
{
	CGF_ASSERT(!m_InPlay, "Scene cannot be started twice");

	m_InPlay = true;

	OnStartActors.Invoke();
}


void Scene::Tick(double dT)
{
	OnTickActors.Invoke(dT);
}


int SceneTypeInfo::TypeCount;