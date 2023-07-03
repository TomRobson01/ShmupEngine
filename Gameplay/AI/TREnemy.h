#pragma once
#include "Objects/TRWorldObject.h"

#include<tuple>
#include <vector>

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

	void SetMotionType(ENEMY_MOTION_TYPE aeMotionType)	{ motionType = aeMotionType; }
	void AddFirePoint(float aiX, float aiY)				{ vFirePoints.push_back(std::make_tuple(aiX, aiY)); }

	ENEMY_MOTION_TYPE const QMotionType() { return motionType; }

protected:
	void FireShot();
	void TickMotion();

private:
	TREnemy() {}

	std::vector<std::tuple<float, float>> vFirePoints;

	ENEMY_MOTION_TYPE motionType;
};

