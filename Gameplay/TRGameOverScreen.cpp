#include "TRGameOverScreen.h"

#include "TRInput.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"
#include "Rendering/TRRenderer.h"

namespace
{
	int iGameOverTexture = -1;
}

TRGameOverScreen* TRGameOverScreen::instancePtr;
TRGameOverScreen* const TRGameOverScreen::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRGameOverScreen();
	}
	return instancePtr;
}

/// <summary>
/// Constructor just loads game over texture.
/// </summary>
TRGameOverScreen::TRGameOverScreen()
{
	iGameOverTexture = TextureLoader::QInstance().RequestTexture("Assets/Textures/T_GameOver.png");
}

/// <summary>
/// Updates the menu. Mainly handles rendering the game over text, and input responses for beginning and ending.
/// </summary>
void TRGameOverScreen::Update()
{
	if (TRWorld::QInstance()->QGameEnded())
	{
		// Render the game over title
		TRRenderer::QInstance().AddRenderTarget(
			0,
			0,
			0,
			0,
			iGameOverTexture,
			8);

		if (TRInput::QInstance()->QKey(TRInputKey::RESTART))
		{
			TRLogger::QInstance()->Log("Restart");
			TRWorld::QInstance()->StartGame();
		}
		if (TRInput::QInstance()->QKey(TRInputKey::QUIT))
		{
			TRWorld::QInstance()->RequestQuit();
		}
	}
}

