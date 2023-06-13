#include "TRProjectile.h"

#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"


TRProjectile::TRProjectile(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	collider = new CircleCollider(afColliderRadius, aeLayer);
	objID = aiID;
}

TRProjectile::~TRProjectile()
{
}

void TRProjectile::OnStart()
{
	this->TRWorldObject::OnStart();
}

void TRProjectile::OnUpdate()
{
	QTransform()->Translate(fXSpeed * FIXED_DELTA_TIME, fYSpeed * FIXED_DELTA_TIME, 0, 0);
	this->TRWorldObject::OnUpdate();
}

void TRProjectile::OnFixedUpdate()
{
	fTimeLived += FIXED_DELTA_TIME;
	if (fTimeLived >= fLifetime)
	{
		Destroy();
	}

	this->TRWorldObject::OnFixedUpdate();
}

void TRProjectile::InitializeProjectileData(float afXSpeed, float afYSpeed, float afLifetime)
{
	fXSpeed = afXSpeed;
	fYSpeed = afYSpeed;
	fLifetime = afLifetime;
}
