#pragma once
#include "TRWorldObject.h"


class TRPlayer : public TRWorldObject
{
public:
	TRPlayer(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TRPlayer();
	static TRPlayer* QInstance();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;


private:
	void CalculateTargetVelocity();
	void HandleShotFired();

	TRPlayer() {}

	static TRPlayer* instancePtr;
};

