#pragma once

#include "TRObject.h"
#include "Physics/TRPhysics.h"

class TRWorldObject : TRObject
{
public:
	TRWorldObject() = default;
	TRWorldObject(TRObject& aBaseObj, Transform atInitialTransform);
	~TRWorldObject();

	void CallStart()		{ OnStart(); }
	void CallUpdate()		{ OnUpdate(); }
	void CallFixedUpdate()	{ OnFixedUpdate(); }

	TRObject QBaseObject() { return baseObject; }

	Transform* QTransform() { return &transform; }

protected:
	virtual void OnStart();
	virtual void OnUpdate();
	virtual void OnFixedUpdate();

private:
	Transform transform;
	TRObject baseObject;
};

