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
		prim = SharedPtr<SpriteBatchComponent>::Create();
		AddComponent(prim);

		SharedPtr<MaterialInstance> material = Game->GetAssetLibrary()->Get<MaterialInstance>("Sprite");
		prim->SetMaterial(material);

		static auto ball2 = prim->CreateSprite();
		ball = prim->CreateSprite();
		
		texture = Game->GetAssetLibrary()->Get<Texture2D>("Atlas");
		
		material->GetFragmentShaderVariable("g_Texture")->Set(texture->GetHandle()->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));

		ball->SetSize({ 640, 480 });
		ball2->SetSize({ 640, 480 });
		ball2->SetPosition({ 10, 10 });
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