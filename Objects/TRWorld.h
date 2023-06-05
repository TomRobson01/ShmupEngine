#pragma once

#include <map>

#include "TRWorldObject.h"
#include "Rendering/TRRenderer.h"

#define FIXED_DELTA_TIME 0.06		

class TRWorld
{
public:
	static TRWorld* QInstance();

	void UpdateWorld();
	void UnloadWorld();

	/// <summary>
	/// Handles creation and initialization for TRWorldObjects, created from a TRObject base.
	/// </summary>
	/// <typeparam name="T">Ensure the type we use inherrits from TRWorldObject.</typeparam>
	/// <param name="aBase">The TRObject we wish to use as a baseline, which the WorldObject will collect data from.</param>
	/// <param name="atInitialTransform">The initial transformation for this new object.</param>
	/// <returns>A stared_ptr of the newly instanciated object.</returns>
	template<typename T>
	std::shared_ptr<T> InstanciateObject(TRObject aBase, Transform atInitialTransform = Transform())
	{
		IDGenerator++;
		std::shared_ptr<T> pOut = std::make_shared<T>(aBase, atInitialTransform, IDGenerator);
		worldObjects.insert(std::make_pair(IDGenerator, pOut));
		pOut->CallStart();
		return pOut;
	}

	/// <summary>
	/// Erases a world object from the worldObjects map with a given key. This despawns the object and removes it from memory.
	/// </summary>
	/// <param name="aiTargetID">The target objects world ID, which the worldObjects map uses as a key.</param>
	void RemoveWorldObject(int aiTargetID)
	{
		worldObjects.erase(aiTargetID);
	}

private:
	TRWorld() {}

	static TRWorld* instancePtr;

	std::map<int, std::shared_ptr<TRWorldObject>> worldObjects;
	int IDGenerator = 0;
};


