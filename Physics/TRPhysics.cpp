#include "TRPhysics.h"

#include <algorithm>
#include <atomic>
#include <map>

#include <iostream>
#include <thread>
#include <vector>

#include "Objects/TRWorldObject.h"

namespace
{
	std::atomic<bool> bShouldStopPhysicsThread;
	std::map<int, TRWorldObject*> CollisionLayers[5];
	int IDGenerator = 1;
}

TRPhysics* TRPhysics::instancePtr;
TRPhysics* TRPhysics::QInstance()
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
bool QIsColliding(TRWorldObject* apColliderA, TRWorldObject* apColliderB)
{
	bool bRetVal = false;

	// Safety checks - don't even try anything if anything essential is nullptr
	if (!apColliderA || !apColliderB || !apColliderA->QCollider() || !apColliderB->QCollider())
		return false;

	CircleCollider colA = *apColliderA->QCollider();
	CircleCollider colB = *apColliderB->QCollider();

	float fLargestRadius = std::max(colA.QRadius(), colB.QRadius());
	float fTangent = abs(colB.QPosY() - colA.QPosY());
	float fAdjacent = abs(colB.QPosX() - colA.QPosX());
	float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));

	if (fDist < fLargestRadius)
	{
		apColliderA->CallOnCollision(apColliderB->QID());
		apColliderB->CallOnCollision(apColliderA->QID());
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
bool QIsCollidingWithAnyInLayer(TRWorldObject* apCollider, CollisionLayer aeTargetlayer)
{
	for (int i = 0; i < CollisionLayers[(int)aeTargetlayer].size(); i++)
	{
		if (CollisionLayers[(int)aeTargetlayer][i])
		{
			if (QIsColliding(apCollider, CollisionLayers[(int)aeTargetlayer][i]))
			{
				return true;
			}
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
				if (CollisionLayers[layer][i])
				{
					//
					// DEFINE ANY LAYER v LAYER COLLISION CHECKS HERE!!!
					//
					switch (layer)
					{
					case (int)CollisionLayer::CL_PLAYER:
						QIsCollidingWithAnyInLayer(CollisionLayers[layer][i], CollisionLayer::CL_ENEMY_PROJECTILE);
						break;
					case (int)CollisionLayer::CL_ENEMY:
						QIsCollidingWithAnyInLayer(CollisionLayers[layer][i], CollisionLayer::CL_PLAYER_PROJECTILE);
						break;
					}
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
	IDGenerator++;
	apCollider->QCollider()->SetID(IDGenerator);
	CollisionLayers[(int)aeTargetLayer].insert(std::make_pair(IDGenerator, apCollider));
}

/// <summary>
/// Removes an object from all collision layers
/// </summary>
/// <param name="apCollider">The parent object we wish to release</param>
void TRPhysics::UnRegisterCollider(TRWorldObject* apCollider)
{
	for (int i = 0; i < (int)CollisionLayer::CL_COUNT; i++)
	{
		CollisionLayers[i].erase(apCollider->QCollider()->QID());
	}
}

CircleCollider::CircleCollider(float afRadius, CollisionLayer aeLayer)
{
	x = 0;
	y = 0;
	fRadius = afRadius;
	colLayer = aeLayer;
}

CircleCollider::CircleCollider(float ax, float ay, float afRadius, CollisionLayer aeLayer)
{
	x = ax;
	y = ay;
	fRadius = afRadius;
	colLayer = aeLayer;
}

