#include "core/Game.h"
#include "core/AssetLibrary.h"
#include "core/Window.h"
#include "graphics/Renderer.h"


GameBase::GameBase()
{
	Game = this;

	m_AssetLibrary = new AssetLibrary;
	m_Window = new Window("Hello", 1920, 1080);
	m_Renderer = new Renderer(m_Window);
}


void GameBase::Start()
{
	
}


void GameBase::Update()
{

}