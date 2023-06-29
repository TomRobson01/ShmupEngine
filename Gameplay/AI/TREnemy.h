#pragma once
#include "Objects/TRWorldObject.h"

enum class ENEMY_MOTION_TYPE
{
	MT_PROGRESS,
	MT_HOMING,
	MT_COUNT
};

class TREnemy : public TRWorldObject
{
public:
	TREnemy(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TREnemy();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnCollision(TRWorldObject* apOtherObject) override;

	void DamageHealth(float afDamage);

	ENEMY_MOTION_TYPE const QMotionType() { return motionType; }

protected:
	void FireShot();
	void TickMotion();

private:
	TREnemy() {}

	ENEMY_MOTION_TYPE motionType;
};

