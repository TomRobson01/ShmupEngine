#pragma once

#include "TRObject.h"
#include "Physics/TRPhysics.h"

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
	//void		CallOnCollision(TRWorldObject* apOtherObject)	{ OnCollision(apOtherObject); }

	TRObject		QBaseObject()	{ return baseObject; }

	Transform*		QTransform()	{ return transform; }
	CircleCollider*	QCollider()		{ return collider;	}

	void		Destroy();

	int QID() { return objID; }

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

	virtual void OnCollision(TRWorldObject* apOtherObject);

	Transform* transform;
	CircleCollider* collider;
	TRObject baseObject;

	int objID;

private:
	int iCollidingObjectID = -1;
};

