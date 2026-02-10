#include "core/Game.h"
#include "core/AssetLibrary.h"
#include "core/Window.h"
#include "core/Scene.h"
#include "core/Input.h"

#include "graphics/Renderer.h"
#include "graphics/Context.h"


GameBase::GameBase()
{
	Game = this;

	m_AssetLibrary = new AssetLibrary("Content.cgfb");
	m_Window = new Window("cgf", 1920, 1080);
	m_GraphicsContext = new GraphicsContext(m_Window);
	m_Input = new Input(m_Window);
	m_Renderer = new Renderer;

	m_Input->SetCursorState(true);
}


void GameBase::Start()
{
	m_CurrentScene->Start();
}


void GameBase::Tick(double dT)
{
	static int frames = 0;
	static double accum = 0.0;
	frames++;
	accum += dT;

	if(frames == 1000)
	{
		int fps = frames / accum;

		CGF_INFO(fps);
		accum = 0.0;
		frames = 0;
	}

	m_Input->NewInputFrame();
	m_CurrentScene->Tick(dT);	
}


void GameBase::Render()
{
	m_Renderer->Render();
}


void GameBase::SetCurrentScene(SharedPtr<Scene> nextScene)
{
	m_CurrentScene = nextScene;
}