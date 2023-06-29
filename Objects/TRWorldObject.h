#pragma once

#include "TRObject.h"
#include "Physics/TRPhysics.h"

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

	TRObject		const QBaseObject() 	{ return baseObject; }

	Transform*		const QTransform()		{ return transform; }
	CircleCollider* const QCollider()		{ return collider;	}
	unsigned int	const QAnimIndex()		{ return uiCurrentAnimIndex; }

	void		Destroy();

	int QID() const { return objID; }

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();
	virtual void OnCollision(TRWorldObject* apOtherObject);

	void OnAnimationUpdate();

	Transform* transform;
	CircleCollider* collider;
	TRObject baseObject;

	int objID;

private:
	int iCollidingObjectID = -1;
	unsigned int uiCurrentAnimIndex = 0;
	clock_t lastAnimationUpdate;
};

