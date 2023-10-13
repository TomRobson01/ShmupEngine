#pragma once
#include "Objects/TRWorldObject.h"

enum class DROP_TYPE
{
	DROP_HEART,
	DROP_TRICANNON,
	DROP_SCREENCLEAR,
	DROP_COUNT
};

class TRDrop : public TRWorldObject
{
public:
	TRDrop(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID);
	~TRDrop();

	void OnUpdate() override;
	void SetDropType(DROP_TYPE aeDropType)	{ dropType = aeDropType; }
	void SetMoveSpeed(float afNewMoveSpeed) { fMoveSpeed = afNewMoveSpeed; }

private:
	TRDrop() {}

	DROP_TYPE dropType = DROP_TYPE::DROP_HEART;
	float fMoveSpeed = 3.0f;
};

