#include "cgf.h"

#include <memory>

#include "core/Memory.h"
#include "core/Events.h"
#include "core/Actor.h"
#include "core/Component.h"
#include "core/Scene.h"

#include "graphics/Diligent.h"

#include "actors/Spectator.h"

#include "utility/Timer.h"

#include "assimp/Importer.hpp" 
#include "assimp/scene.h"
#include "assimp/postprocess.h"


class MeshActor : public Actor
{
public:
	MeshActor()
	{
		prim = SharedPtr<MeshComponent>::Create();
		AddComponent(prim);

		SharedPtr<MaterialInstance> material = Game->GetAssetLibrary()->Get<MaterialInstance>("Green");
		prim->SetMaterial(material);

		material->GetBaseMaterial()->SetFillMode(Diligent::FILL_MODE_WIREFRAME);

		SharedPtr<StaticMesh> mesh = Game->GetAssetLibrary()->Get<StaticMesh>("Icosphere");
		prim->SetMesh(mesh);

		prim->Transform.Scale = glm::vec3(1);
		prim->Transform.Rotation = glm::vec3(-3.1415f / 2.0f, 0.0f, 0.0f);
	}

	void Tick(double dT) override
	{
		float speed = 1.f;

		prim->Transform.Rotate(0, speed * dT, 0);

		Actor::Tick(dT);
	}

	SharedPtr<MeshComponent> prim;
};


class DevScene : public Scene
{
public:
	DevScene()
	{
		SharedPtr<MeshActor> actor = SharedPtr<MeshActor>::Create();
		AddActor(actor);

		CurrentCamera->Transform.LookAt(glm::vec3(0));

		SharedPtr<Spectator> e = SharedPtr<Spectator>::Create();
		AddActor(e);
	}
};


template<typename SceneT>
class MinimalGame : public GameBase
{
	static_assert(std::is_base_of_v<Scene, SceneT>,
		"SceneT must publicly derive Scene");

public:
	MinimalGame()
	{
		SetCurrentScene(SharedPtr<SceneT>::Create());
	}
};


int main()
{
	GameBase *game = new MinimalGame<DevScene>;
	game->Start();

	Timer frameTimer;

	while (!game->GetWindow()->ShouldClose())
	{
		game->GetWindow()->Poll();

		game->Tick(frameTimer.GetElapsed());
		frameTimer.Restart();
		
		game->Render();
	}

	delete game;
	return 0;
}