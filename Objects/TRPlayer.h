#pragma once
#include "TRWorldObject.h"


class TRPlayer : public TRWorldObject
{
public:
	TRPlayer(TRObject& aBaseObj, Transform atInitialTransform);
	~TRPlayer();
	static TRPlayer* QInstance();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;


private:
	void CalculateTargetVelocity();

	TRPlayer() {}

	static TRPlayer* instancePtr;
};

