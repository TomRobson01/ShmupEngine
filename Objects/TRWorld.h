#pragma once
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
		std::shared_ptr<T> pOut = std::make_shared<T>(aBase, atInitialTransform);
		AddWorldObject(pOut);
		return pOut;
	}

protected:
	void AddWorldObject(std::shared_ptr<TRWorldObject> apWorldObj);

private:
	TRWorld() {}

	static TRWorld* instancePtr;
};


