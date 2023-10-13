#pragma once
#include "Objects/TRWorldObject.h"

#include<tuple>
#include <vector>

enum class ENEMY_MOTION_TYPE
{
	MT_PROGRESS,
	MT_SINE,
	MT_HOMING,
	MT_COUNT
};

struct FirePoint
{
	FirePoint(const float afFirePointX, const float afFirePointY, const float afFireDirX = -1.0f, const float afFireDirY = 0.0f)
	{
		fFirePointX = afFirePointX;
		fFirePointY = afFirePointY;
		fFireDirectionX = afFireDirX;
		fFireDirectionY = afFireDirY;
	}

	float fFirePointX;
	float fFirePointY;
	float fFireDirectionX;
	float fFireDirectionY;
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

	void SetHealth(float afHealth)															{ fHealth = afHealth; }
	void SetMoveSpeed(float afSpeed)														{ fMoveSpeed = afSpeed; }
	void SetProjectileSpeed(float afSpeed)													{ fProjectileSpeed = afSpeed; }
	void SetMotionType(ENEMY_MOTION_TYPE aeMotionType)										{ motionType = aeMotionType; }
	void SetSineAmplitude(float afNewAmplitude)												{ fSineAmplitude = afNewAmplitude; }
	void AddFirePoint(float aiX, float aiY, float aiDirX = -1.0f, float aiDirY = 0.0f)		{ vFirePoints.push_back(FirePoint(aiX, aiY, aiDirX, aiDirY)); }

	ENEMY_MOTION_TYPE const QMotionType() { return motionType; }

protected:
	void FireShot();
	void TickMotion();

private:
	TREnemy() {}

	std::vector<FirePoint> vFirePoints;

	ENEMY_MOTION_TYPE motionType;

	float fHealth = 10.0f;
	float fCurrentFireCooldown = 0.0f;
	float fTimeSinceFire = 0.0f;
	float fMoveSpeed = 1.0f;
	float fProjectileSpeed = 5.5f;
	float fTime = 0.0f;
	float fSineAmplitude = 1.0f;
};

