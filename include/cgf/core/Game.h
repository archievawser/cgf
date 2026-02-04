#pragma once

#include "core/Common.h"
#include "core/Memory.h"
#include "core/Architecture.h"

#include "graphics/Context.h"


class AssetLibrary;
class Renderer;
class Window;


/**
 * @brief 
 */
class GameBase
{
public:
	GameBase();

	/**
	 * @brief Called immediately after initialization and before the first call to Update()
	 */
	virtual void Start();

	/**
	 * @brief Called every frame, prior to rendering
	 */
	virtual void Update();

	/**
	 * @return The current Renderer
	 */
	FORCEINLINE Renderer* GetRenderer() const
	{
		return m_Renderer;
	}

	/**
	 * @return The current AssetLibrary
	 */
	FORCEINLINE AssetLibrary* GetAssetLibrary() const
	{
		return m_AssetLibrary;
	}

	FORCEINLINE Window* GetWindow()
	{
		return m_Window;
	}

	FORCEINLINE SharedPtr<Scene> GetCurrentScene()
	{
		return m_CurrentScene;
	}

	FORCEINLINE GraphicsContext* GetGraphicsContext()
	{
		return m_GraphicsContext;
	}

private:
	Window* m_Window;
	AssetLibrary* m_AssetLibrary;
	Renderer* m_Renderer;
	GraphicsContext* m_GraphicsContext;
	SharedPtr<Scene> m_CurrentScene;
};


inline GameBase* Game;