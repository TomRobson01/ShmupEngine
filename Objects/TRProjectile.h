#pragma once
#include "TRWorldObject.h"

class TRProjectile : public TRWorldObject
{
public:
	TRProjectile(TRObject& aBaseObj, Transform atInitialTransform, int aiID);
	~TRProjectile();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;

	void InitializeProjectileData(float afXSpeed, float afYSpeed, float afLifetime);

private:
	TRProjectile(){};

	float fLifetime		= 99.0f;
	float fTimeLived	= 0.0f;
	float fXSpeed		= 1.0f;
	float fYSpeed		= 0.0f;
};

