#include "TRWorld.h"

#include <iostream>

#include <ctime>
#include <vector>

#include "Gameplay/AI/TRWaveManager.h"
#include "Gameplay/TRGameOverScreen.h"
#include "Gameplay/TRMainMenu.h"
#include "Objects/TRPlayer.h"


namespace 
{
	clock_t lastAnimationUpdate;
	clock_t lastFixedUpdateTime;
	clock_t camShakeStarted;
	std::vector<int> garbageObjCollection;
}

TRWorld* TRWorld::instancePtr;
TRWorld* const TRWorld::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRWorld();
	}
	return instancePtr;
}

/// <summary>
/// Handles per-frame, and fixed count, updates for all currently held TRWorldObjects.
/// </summary>
void TRWorld::UpdateWorld()
{
	TRMainMenu::QInstance()->Update();

	if (bGameEnded && bGameStarted)
	{
		TRGameOverScreen::QInstance()->Update();
	}

	// Get the time since last Update tick
	const clock_t cCurrentClock = clock();
	float fTimeSinceFixedUpdate = cCurrentClock - lastFixedUpdateTime;

	if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
	{
		lastFixedUpdateTime = cCurrentClock;
	}

	// Resolve camera shake
	float fTimeSinceCamShakeBegin = cCurrentClock - camShakeStarted;
	if (fTimeSinceCamShakeBegin > fCamShakeDuration)
	{
		TRRenderer::QInstance().SetCameraShake(false);
	}

	// Before updating anything remove any object we marked for deferred removal
	for (int i : garbageObjCollection)
	{
		TRWorld::QInstance()->objLock.lock();
		worldObjects.erase(i);
		TRWorld::QInstance()->objLock.unlock();
	}

	// Send CallUpdate() to every object, and then if it's a FixedUpdate frame, send CallFixedUpdate()
	for (int i = 0; i < worldObjects.size(); i++)
	{
		TRWorld::QInstance()->objLock.lock();
		if (worldObjects[i])
		{
			worldObjects[i]->CallUpdate();
			if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
			{
				worldObjects[i]->CallFixedUpdate();
			}
		}
		TRWorld::QInstance()->objLock.unlock();
	}

	if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
	{
		TRWaveManager::QInstance()->Update();
	}

}

/// <summary>
/// Releases our references to all WorldObjects from memory. 
/// NOTE: If another class references a TRWorldObject, it is responsible for releasing it too.
/// </summary>
void TRWorld::UnloadWorld()
{
	worldObjects.clear();
}

std::shared_ptr<TRWorldObject> TRWorld::GetWorldObjectByID(int aiID)
{
	return worldObjects[aiID];
}

/// <summary>
/// Mark an object to be deleted at the beginning of next frame. This is the safest way of deleting an object.
/// </summary>
/// <param name="aiTargetID">The ID of the object we want to remove.</param>
void TRWorld::RemoveWorldObject(int aiTargetID)
{
	garbageObjCollection.push_back(aiTargetID);
}

/// <summary>
/// Sets the duration of a camera shake effect.
/// </summary>
/// <param name="afDuration">Duration in seconds of the camera shake effect.</param>
void TRWorld::SetCamShakeDuration(float afDuration)
{
	fCamShakeDuration = afDuration * 1000;
	const clock_t cCurrentClock = clock();
	camShakeStarted = cCurrentClock;
	TRRenderer::QInstance().SetCameraShake(true);
}

/// <summary>
/// Spawns a player and begins the game started state.
/// </summary>
void TRWorld::StartGame()
{
	InstanciateObject<TRPlayer>(TRWorld::QInstance()->QObjPlayer(), Transform(0, 0, 0, 0), 1.0f, CollisionLayer::CL_PLAYER);
	bGameStarted = true;
	bGameEnded = false;
}

/// <summary>
/// Despawns the player from the world and enters the game ended state.
/// </summary>
void TRWorld::EndGame()
{
	RemoveWorldObject(TRPlayer::QInstance()->QID());
	bGameEnded = true;
}
