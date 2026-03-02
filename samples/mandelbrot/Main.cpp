#include "cgf.h"

#include <memory>

#include "core/Memory.h"
#include "core/Events.h"
#include "core/Actor.h"
#include "core/Component.h"
#include "core/Scene.h"
#include "core/Input.h"

#include "components/SpriteComponent.h"

#include "graphics/Diligent.h"
#include "graphics/Texture.h"

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
		prim = SharedPtr<DynamicMeshComponent>::Create();
		AddComponent(prim);

		SharedPtr<MaterialInstance> material = Game->GetAssetLibrary()->Get<MaterialInstance>("Mandelbrot");
		prim->SetMaterial(material);
		
		material->GetBaseMaterial()->SetCullMode(CULL_MODE_NONE);

		unsigned int* indexData = new unsigned int[] {
			0, 1, 2,
			2, 3, 0
		};

		glm::vec2 screenSize (1920 / 2, 1080 / 2);

		Vertex quadVertices[] 
		{
			{
				.Position = glm::vec3(0, 0, 0) + glm::vec3(-screenSize.x, -screenSize.y, 0),
				.Normal = glm::vec3(0, 0, 0),
				.UV0 = glm::vec3(0, 0, 0),
			},
			{
				.Position = glm::vec3(0, 0, 0) + glm::vec3(screenSize.x, -screenSize.y, 0),
				.Normal = glm::vec3(0, 0, 0),
				.UV0 = glm::vec3(1, 0, 0),
			},
			{
				.Position = glm::vec3(0, 0, 0) + glm::vec3(screenSize.x, screenSize.y, 0),
				.Normal = glm::vec3(0, 0, 0),
				.UV0 = glm::vec3(1, 1, 0),
			},
			{
				.Position = glm::vec3(0, 0, 0) + glm::vec3(-screenSize.x, screenSize.y, 0),
				.Normal = glm::vec3(0, 0, 0),
				.UV0 = glm::vec3(0, 1, 0),
			},
		};

		prim->SetIndexData(indexData, 6);
		prim->SetVertexData(quadVertices, sizeof(Vertex), 4);
	}

	void Tick(double dT) override
	{
		

		Actor::Tick(dT);
	}

	float ballXVelocity = 1.0f;
	float ballYVelocity = 0.0f;
	SharedPtr<Texture2D> texture;
	PooledPtr<SpriteState> ball;
	SharedPtr<SpriteBatchComponent> prim;
};


class DevScene : public Scene
{
public:
	DevScene()
	{
		SharedPtr<MeshActor> actor = SharedPtr<MeshActor>::Create();
		AddActor(actor);

		CurrentCamera->Transform.LookAt(glm::vec3(0));

		//SharedPtr<Spectator> e = SharedPtr<Spectator>::Create();
		//AddActor(e);
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