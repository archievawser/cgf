#include "core/Game.h"
#include "core/AssetLibrary.h"
#include "core/Window.h"

#include "graphics/Renderer.h"
#include "graphics/Context.h"


GameBase::GameBase()
{
	Game = this;

	m_AssetLibrary = new AssetLibrary("Content.cgfb");
	m_Window = new Window("cgf", 1920, 1080);
	m_GraphicsContext = new GraphicsContext(m_Window);
	m_Renderer = new Renderer;
	m_CurrentScene = SharedPtr<Scene>::CreateTraced("Scene");
}


void GameBase::Start()
{
	m_CurrentScene->OnEntityStart.Invoke();
}


void GameBase::Tick(double dT)
{
	m_CurrentScene->OnEntityTick.Invoke(0.0);
}


void GameBase::Render()
{
	m_Renderer->Render();
}