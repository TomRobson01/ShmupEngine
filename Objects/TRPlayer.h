#pragma once
#include "TRWorldObject.h"


class TRPlayer : public TRWorldObject
{
public:
	TRPlayer(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TRPlayer();
	static TRPlayer* const QInstance();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;
	void OnCollision(TRWorldObject* apOtherObject) override;

	void RestoreHealth(int aiHealAmount);
	void TakeDamage(int aiDamage);

	void RequestTricannon();

private:
	void CalculateTargetVelocity();
	void HandleShotFired();

	TRPlayer() {}

	static TRPlayer* instancePtr;

	bool bTriCannonActive;
};

