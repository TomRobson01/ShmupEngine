#include "TRWorld.h"

#include <ctime>
#include <vector>

#define FIXED_UPDATE_TICKS 16.666f	// CLOCKS_PER_SEC is 1000, / 60 target fixed updates a second, we get 16.6 recurring

namespace 
{
	std::vector<std::shared_ptr<TRWorldObject>> worldObjects;
	clock_t lastFixedUpdateTime;
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

	// Send CallUpdate() to every object, and then if it's a FixedUpdate frame, send CallFixedUpdate()
	for (std::shared_ptr<TRWorldObject> obj : worldObjects)
	{
		obj->CallUpdate();
		if (fTimeSinceFixedUpdate >= FIXED_UPDATE_TICKS)
		{
			obj->CallFixedUpdate();
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

/// <summary>
/// Adds a shared_ptr TRWorldObject to our WorldObjects vector.
/// </summary>
/// <param name="apWorldObj">A shared_ptr for the new object we wish to track to the world.</param>
void TRWorld::AddWorldObject(std::shared_ptr<TRWorldObject> apWorldObj)
{
	worldObjects.push_back(apWorldObj);
	apWorldObj->CallStart();
}
