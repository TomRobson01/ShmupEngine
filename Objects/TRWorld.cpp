#include "TRWorld.h"

#include <iostream>

#include <ctime>
#include <vector>

#define FIXED_UPDATE_TICKS 16.666f	// CLOCKS_PER_SEC is 1000, / 60 target fixed updates a second, we get 16.6 recurring

namespace 
{
	clock_t lastFixedUpdateTime;
	std::vector<int> garbageObjCollection;
}

TRWorld* TRWorld::instancePtr;
TRWorld* TRWorld::QInstance()
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
		worldObjects.erase(i);
	}
	garbageObjCollection.clear();

	// Send CallUpdate() to every object, and then if it's a FixedUpdate frame, send CallFixedUpdate()
	for (int i = 0; i < worldObjects.size(); i++)
	{
		if (worldObjects[i])
		{
			worldObjects[i]->CallUpdate();
			if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
			{
				worldObjects[i]->CallFixedUpdate();
			}
		}
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
