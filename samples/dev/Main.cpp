#include <iomanip>
#include <iostream>

#include "core/Common.h"
#include "core/Game.h"
#include "core/Window.h"

#include "graphics/Renderer.h"

#define IMPLEMENT_GAME_ENTRY_POINT(gameClass)     \
	int main()                                    \
	{                                             \
		GameBase *yeah = new gameClass;           \
		yeah->Start();                            \
                                                  \
		while (!yeah->GetWindow()->ShouldClose()) \
		{                                         \
			yeah->GetWindow()->Poll();            \
                                                  \
			yeah->Update();                       \
			yeah->GetRenderer()->Render();        \
		}                                         \
                                                  \
		delete yeah;                              \
		return 0;                                 \
	}

class GameDerivative : public GameBase
{
public:
	void Start() override
	{
		std::cerr << "hi" << std::endl;
	}
};

IMPLEMENT_GAME_ENTRY_POINT(GameDerivative)