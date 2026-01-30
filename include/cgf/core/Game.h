#pragma once

#include "core/Common.h"


class AssetLibrary;
class Renderer;
class Window;


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

private:
	Window* m_Window;
	AssetLibrary* m_AssetLibrary;
	Renderer* m_Renderer;
};


inline GameBase* Game;