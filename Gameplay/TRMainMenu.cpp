#include "TRMainMenu.h"

#include "TRInput.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"
#include "Rendering/TRRenderer.h"

namespace
{
	bool bGameStarted = false;
	int iGameOverTexture = -1;
}

TRMainMenu* TRMainMenu::instancePtr;
TRMainMenu* const TRMainMenu::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRMainMenu();
	}
	return instancePtr;
}

/// <summary>
/// Constructor just loads title texture.
/// </summary>
TRMainMenu::TRMainMenu()
{
	iGameOverTexture = TextureLoader::QInstance().RequestTexture("Assets/Textures/T_Title.png");
}

/// <summary>
/// Updates the menu. Mainly handles rendering the title text, and input responses for beginning and ending.
/// </summary>
void TRMainMenu::Update()
{
	if (!bGameStarted)
	{
		// Render the main title
		TRRenderer::QInstance().AddRenderTarget(
			0,
			0,
			0,
			0,
			iGameOverTexture,
			8);

		if (TRInput::QInstance()->QKey(TRInputKey::START))
		{
			bGameStarted = true;
			TRLogger::QInstance()->Log("Start");
			TRWorld::QInstance()->StartGame();
		}
		if (TRInput::QInstance()->QKey(TRInputKey::QUIT))
		{
			TRWorld::QInstance()->RequestQuit();
		}
	}
}