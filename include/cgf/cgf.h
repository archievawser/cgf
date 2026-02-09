#pragma once

#include "core/Common.h"
#include "core/Game.h"
#include "core/Window.h"

#include "graphics/Renderer.h"

#define IMPLEMENT_GAME_ENTRY_POINT(gameClass)     \
	int main()                                    \
	{                                             \
		GameBase *game = new gameClass;           \
		game->Start();                            \
                                                  \
		while (!game->GetWindow()->ShouldClose()) \
		{                                         \
			game->GetWindow()->Poll();            \
                                                  \
			game->Tick(0.0);                      \
			game->GetRenderer()->Render();        \
		}                                         \
                                                  \
		delete game;                              \
		return 0;                                 \
	}