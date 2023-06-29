#pragma once

#include <map>
#include <memory>

#include "TRDefines.h"
#include "TRWorldObject.h"
#include "Rendering/TRRenderer.h"

class TRWorld
{
public:
	static TRWorld* const QInstance();

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

	TRObject QObjPlayer()	{ return ObjPlayer;		}
	TRObject QObjEnemy()	{ return ObjEnemy;		}

private:
	TRWorld() 
	{
		ObjPlayer	= TRObject("Player",	"Assets/Textures/T_Temp_Player_Sprite.png");
		ObjEnemy	= TRObject("Enemy",		"Assets/Textures/T_Temp_Player_Sprite.png");
	}

	static TRWorld* instancePtr;

	std::map<int, std::shared_ptr<TRWorldObject>> worldObjects;
	int IDGenerator = 0;

	// Usable TRObjects are defined here. You will need to define them in the TRWorld constructor, and add an accessory for them in the public scope
	TRObject ObjPlayer;
	TRObject ObjEnemy;
	// End of TRObject defs ------------------
};


