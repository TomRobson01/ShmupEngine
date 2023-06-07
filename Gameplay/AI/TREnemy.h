#pragma once
#include "Objects/TRWorldObject.h"

class TREnemy : public TRWorldObject
{
public:
	TREnemy(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TREnemy();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnCollision(TRWorldObject* apOtherObject) override;

private:
	TREnemy() {}
};

