#include "TRPhysics.h"

#include <algorithm>
#include <map>

#include <iostream>
#include <vector>

#include "Objects/TRWorldObject.h"

namespace
{
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

/// <summary>
/// Checks if two objects are colliding.
/// </summary>
/// <param name="apColliderA"></param>
/// <param name="apColliderB"></param>
/// <returns>True if the objects are colliding, false if not.</returns>
bool TRPhysics::QIsColliding(TRWorldObject* apColliderA, TRWorldObject* apColliderB)
{
	bool bRetVal = false;

	// Safety checks - don't even try anything if anything essential is nullptr
	if (!apColliderA || !apColliderB || !apColliderA->QCollider() || !apColliderB->QCollider())
		return false;

	CircleCollider colA = *apColliderA->QCollider();
	CircleCollider colB = *apColliderB->QCollider();

	float fLargestRadius = std::max(colA.QRadius(), colB.QRadius());
	float fTangent	= abs(colB.QPosY() - colA.QPosY());
	float fAdjacent = abs(colB.QPosX() - colA.QPosX());
	float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));

	if (fDist < fLargestRadius)
	{
		apColliderA->CallOnCollision(apColliderB);
		apColliderB->CallOnCollision(apColliderA);
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
bool TRPhysics::QIsCollidingWithAnyInLayer(TRWorldObject* apCollider, CollisionLayer aeTargetlayer)
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

