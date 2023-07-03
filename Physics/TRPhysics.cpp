#include "TRPhysics.h"

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>

#include <iostream>
#include <thread>
#include <vector>

#include "Objects/TRWorld.h"
#include "Objects/TRWorldObject.h"

namespace
{
	std::atomic<bool> bShouldStopPhysicsThread;
	std::map<int, TRWorldObject*> CollisionLayers[6];
	int IDGenerator = 1;
}

TRPhysics* TRPhysics::instancePtr;
TRPhysics* const TRPhysics::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRPhysics();
	}
	return instancePtr;
}

/// <summary>
/// Checks if two objects are colliding.
/// </summary>
/// <param name="apColliderA"></param>
/// <param name="apColliderB"></param>
/// <returns>True if the objects are colliding, false if not.</returns>
/// <remarks> DEPRICATED - Objects will now no longer be able to query collision with specific objects. All collision checks are handled by the Collision Thead. </remarks>
bool QIsColliding(CollisionData apColliderA, CollisionData apColliderB)
{
	bool bRetVal = false;

	// Safety checks - don't even try anything if anything essential is nullptr
	/*if (!apColliderA || !apColliderB)
		return false;*/

	float fColA_X = apColliderA.x;
	float fColA_Y = apColliderA.y;
	float fColA_Radius = apColliderA.radius;

	float fColB_X = apColliderB.x;
	float fColB_Y = apColliderB.y;
	float fColB_Radius = apColliderB.radius;

	float fLargestRadius = std::max(fColA_Radius, fColB_Radius);
	float fTangent = abs(fColB_Y - fColA_Y);
	float fAdjacent = abs(fColB_X - fColA_X);
	float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));

	if (fDist < fLargestRadius)
	{
		std::cout << "Colliding! \n";
		bRetVal = true;
	}

	return bRetVal;
}

/// <summary>
/// Checks if an object is colliding with any object on a layer.
/// </summary>
/// <param name="apCollider">The object we wish to check against all other objects.</param>
/// <param name="aeTargetlayer">The layer we want to query.</param>
/// <returns>True if any collisions are found.</returns>
/// <remarks>This will send the OnCollision event to the first object detected. This is for optimisation. If we find a use, I may add a seperate function that will sweep all objects and notify them all.</remarks>
/// <remarks> DEPRICATED - Objects will now no longer be able to query collision with specific objects. All collision checks are handled by the Collision Thead. </remarks>
bool QIsCollidingWithAnyInLayer(CollisionData apColliderA, CollisionLayer aeColliderALayer, int aiColliderAID, CollisionLayer aeTargetlayer)
{
	for (int i = 0; i < CollisionLayers[(int)aeTargetlayer].size(); i++)
	{
		TRWorld::QInstance()->objLock.lock();
		if (CollisionLayers[(int)aeTargetlayer][i])
		{
			// Quickly cache our colliderB data while we have the lock
			CollisionData colBData = CollisionData();
			colBData.x = CollisionLayers[(int)aeTargetlayer][i]->QCollisionX();
			colBData.y = CollisionLayers[(int)aeTargetlayer][i]->QCollisionY();
			colBData.radius = CollisionLayers[(int)aeTargetlayer][i]->QCollisionRadius();
			colBData.ID = CollisionLayers[(int)aeTargetlayer][i]->QID();
			TRWorld::QInstance()->objLock.unlock();

			// Now we have our data cached, release the lock and do the collision check
			if (QIsColliding(apColliderA, colBData))
			{
				TRWorld::QInstance()->objLock.lock();
				if (CollisionLayers[(int)aeTargetlayer][i] && CollisionLayers[(int)aeColliderALayer][aiColliderAID])
				{
					CollisionLayers[(int)aeColliderALayer][aiColliderAID]->CallOnCollision(colBData.ID);	// Object A is notified of collision with object B
					CollisionLayers[(int)aeTargetlayer][i]->CallOnCollision(apColliderA.ID);				// Object B is notified of collision with object A
				}
				TRWorld::QInstance()->objLock.unlock();
				return true;
			}
		}
		else
		{
			TRWorld::QInstance()->objLock.unlock();
		}
	}
	return false;
}

/// <summary>
/// Until told otherwise, loop through all objects in each physics layer and check if they're colliding with anything on their target layers.
/// </summary>
void CollisionThread()
{
	while (!bShouldStopPhysicsThread)
	{
		for (int layer = 0; layer < (int)CollisionLayer::CL_COUNT; layer++)
		{
			for (int i = 0; i < CollisionLayers[layer].size(); i++)
			{
				TRWorld::QInstance()->objLock.lock();
				if (CollisionLayers[layer][i])
				{
					// Cache our collision data for the current object, while we have a lock
					CollisionData colData = CollisionData();
					colData.x = CollisionLayers[layer][i]->QCollisionX();
					colData.y = CollisionLayers[layer][i]->QCollisionY();
					colData.radius = CollisionLayers[layer][i]->QCollisionRadius();
					colData.ID = CollisionLayers[layer][i]->QID();
					TRWorld::QInstance()->objLock.unlock();

					//
					// DEFINE ANY LAYER v LAYER COLLISION CHECKS HERE!!!
					//
					switch (layer)
					{
					case (int)CollisionLayer::CL_PLAYER:
						QIsCollidingWithAnyInLayer(colData, (CollisionLayer)layer, i, CollisionLayer::CL_ENEMY_PROJECTILE);
						QIsCollidingWithAnyInLayer(colData, (CollisionLayer)layer, i, CollisionLayer::CL_ENEMY_SUICIDER);
						break;
					case (int)CollisionLayer::CL_ENEMY:
					case (int)CollisionLayer::CL_ENEMY_SUICIDER:
						QIsCollidingWithAnyInLayer(colData, (CollisionLayer)layer, i, CollisionLayer::CL_PLAYER_PROJECTILE);
						break;
					}
				}
				else
				{
					TRWorld::QInstance()->objLock.unlock();
				}
			}

		}
		
	}
}

/// <summary>
/// Kicks off the Collision Thread. 
/// This thread will run for the lifetime of the program, and inform any colliders on the main thread if they've collided with anything on their layer
/// </summary>
void TRPhysics::InitCollisionThread()
{
	std::thread ColThread(CollisionThread);
	ColThread.detach();
}

/// <summary>
/// Atomically tells the collision thread to stop processing after it's current itteration.
/// </summary>
void TRPhysics::ShutdownCollisionThread()
{
	bShouldStopPhysicsThread = true;
}

/// <summary>
/// Adds a new object to a given collision layer
/// </summary>
/// <param name="aeTargetLayer">The layer we wish to add the object to</param>
/// <param name="apCollider">The object we wish to add to that layer</param>
void TRPhysics::RegisterCollider(CollisionLayer aeTargetLayer, TRWorldObject* apCollider)
{
	CollisionLayers[(int)aeTargetLayer].insert(std::make_pair(apCollider->QID(), apCollider));
}

/// <summary>
/// Removes an object from all collision layers
/// </summary>
/// <param name="apCollider">The parent object we wish to release</param>
void TRPhysics::UnRegisterCollider(TRWorldObject* apCollider)
{
	for (int i = 0; i < (int)CollisionLayer::CL_COUNT; i++)
	{
		CollisionLayers[i].erase(apCollider->QID());
	}
}

