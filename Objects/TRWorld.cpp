#include "TRWorld.h"

#include <iostream>

#include <ctime>
#include <vector>

#include "Gameplay/AI/TRWaveManager.h"


namespace 
{
	clock_t lastFixedUpdateTime;
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

	// Get the time since last Update tick
	const clock_t cCurrentClock = clock();
	float fTimeSinceFixedUpdate = cCurrentClock - lastFixedUpdateTime;

	if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
	{
		lastFixedUpdateTime = cCurrentClock;
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
