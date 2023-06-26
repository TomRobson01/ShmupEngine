#pragma once

#include <map>
#include <memory>

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
	std::shared_ptr<T> InstanciateObject(TRObject aBase, Transform atInitialTransform = Transform(), float afColliderRadius = 0.5f, CollisionLayer aeLayer = CollisionLayer::CL_DEFAULT)
	{
		IDGenerator++;
		std::shared_ptr<T> pOut = std::make_shared<T>(aBase, atInitialTransform, afColliderRadius, aeLayer, IDGenerator);
		worldObjects.insert(std::make_pair(IDGenerator, pOut));
		pOut->CallStart();
		TRPhysics::QInstance()->RegisterCollider(aeLayer, pOut.get());
		return pOut;
	}

	std::shared_ptr<TRWorldObject> GetWorldObjectByID(int aiID);
	void RemoveWorldObject(int aiTargetID);

private:
	TRWorld() {}

	static TRWorld* instancePtr;

	std::map<int, std::shared_ptr<TRWorldObject>> worldObjects;
	int IDGenerator = 0;
};


