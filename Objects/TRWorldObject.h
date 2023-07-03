#pragma once

#include "TRObject.h"
#include "Physics/TRPhysics.h"

#include <atomic>
#include <ctime>

class TRWorldObject : public TRObject
{
public:
	TRWorldObject() = default;
	TRWorldObject(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TRWorldObject();

	void		CallStart()			{ OnStart(); }
	void		CallUpdate()		{ OnUpdate(); }
	void		CallFixedUpdate()	{ OnFixedUpdate(); }
	void		CallOnCollision(int aiCollidingObjectID);

	TRObject		const QBaseObject() 		{ return baseObject; }

	Transform*		const QTransform()			{ return transform; }
	unsigned int	const QAnimIndex()			{ return uiCurrentAnimIndex; }
	float			const QCollisionX()			{ return fCollisionX; }
	float			const QCollisionY()			{ return fCollisionY; }
	float			const QCollisionRadius()	{ return fCollisionRadius; }
	CollisionLayer	const QCollisionLayer()		{ return eCollisionLayer; }

	void			Destroy();
	void			Destroy(TRObject aoExplosionObject);


	void SetAnimLoop(bool abLoops = true)						{ bLoopsAnim = abLoops; }
	void SetExplosionCollisionLayer(CollisionLayer aeColLayer)	{ eExplosionCollisionLayer = aeColLayer; };

	int QID() const { return objID; }


protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();
	virtual void OnCollision(TRWorldObject* apOtherObject);

	void OnAnimationUpdate();

	Transform* transform;
	std::atomic<float> fCollisionX;
	std::atomic<float> fCollisionY;
	std::atomic<float> fCollisionRadius;
	CollisionLayer eCollisionLayer = CollisionLayer::CL_DEFAULT;
	TRObject baseObject;

	int objID;

private:
	int iCollidingObjectID = -1;
	unsigned int uiCurrentAnimIndex = 0;
	clock_t lastAnimationUpdate;
	bool bLoopsAnim = true;
	CollisionLayer eExplosionCollisionLayer = CollisionLayer::CL_DEFAULT;
};

