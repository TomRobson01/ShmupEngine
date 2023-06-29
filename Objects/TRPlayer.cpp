#include "TRPlayer.h"

#include <iostream>

#include "TRInput.h"
#include "TRProjectile.h"
#include "TRWorldObject.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"

#define PLAYER_SPEED 5	// We'll want this to be exposed to the editor eventually, but until we know what form that takes, let's just define it here
#define CLOCKS_BETWEEN_SHOTS 5

namespace
{
	ImVec2 vCurrentVelocity;	// Our current velocity - interpolates towards vTargetVelocity per fixed update
	ImVec2 vTargetVelocity;		// Our "wish" velocity

	const ImVec2 vPlayerXClamp = ImVec2(-15.5f, -2.0f);
	const ImVec2 vPlayerYClamp = ImVec2(-11.5f, 11.5f);

	int iTicksSinceShot		= 0;
	bool bCanShoot			= false;

	TRObject ObjProjectile;
}

TRPlayer* TRPlayer::instancePtr;
TRPlayer* const TRPlayer::QInstance()
{
	return instancePtr;
}

TRPlayer::TRPlayer(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	collider = new CircleCollider(afColliderRadius, aeLayer);
	objID = aiID;
	instancePtr = this;
}

TRPlayer::~TRPlayer()
{
}

void TRPlayer::OnStart()
{
	ObjProjectile = TRObject("Projectile", "Assets/Textures/T_Temp_Player_Sprite.png");
	QTransform()->SetClamp(vPlayerXClamp.x, vPlayerXClamp.y, vPlayerYClamp.x, vPlayerYClamp.y);

	TRLoggerImGui::QInstance()->AddLog("New player started!", LogSeverity::TR_DEFAULT);
	this->TRWorldObject::OnStart();
}

void TRPlayer::OnUpdate()
{
	// Movement 
	CalculateTargetVelocity();
	QTransform()->Translate(vTargetVelocity.x, vTargetVelocity.y, 0, 0);

	// Combat
	if (TRInput::QInstance()->QMouseHeld(TRMOUSE_LEFT) && bCanShoot)
	{
		// Shoot!
		HandleShotFired();
	}

	this->TRWorldObject::OnUpdate();
}

void TRPlayer::OnFixedUpdate()
{
	// Tick the shot cooldown up, and handle the current state of that
	// We may want to replace this with a proper timer class later, depends on engine scope
	if (!bCanShoot)
	{
		iTicksSinceShot++;
		if (iTicksSinceShot >= CLOCKS_BETWEEN_SHOTS)
		{
			bCanShoot = true;
		}
	}
	this->TRWorldObject::OnFixedUpdate();
}

void TRPlayer::OnCollision(TRWorldObject* apOtherObject)
{
	if (!apOtherObject)
		return;

	switch (apOtherObject->QCollider()->QCollisionLayer())
	{
	case CollisionLayer::CL_ENEMY_PROJECTILE:
		TRWorld::QInstance()->RemoveWorldObject(apOtherObject->QID());
		TRLoggerImGui::QInstance()->AddLog("Player hit!", LogSeverity::TR_DEFAULT);
		break;
	default:
		break;
	}
}

/// <summary>
/// Calculates the targt velocity based on the current user inputs
/// </summary>
void TRPlayer::CalculateTargetVelocity()
{
	// First collect the input vector
	vTargetVelocity = ImVec2(0, 0);	// Reset target each frame, we want to calculate it from scratch based on our current inputs
	if (TRInput::QInstance()->QKey(TRInputKey::UP))
	{
		vTargetVelocity.y = 1;
	}
	if (TRInput::QInstance()->QKey(TRInputKey::DOWN))
	{
		vTargetVelocity.y = -1;
	}
	if (TRInput::QInstance()->QKey(TRInputKey::LEFT))
	{
		vTargetVelocity.x = -1;
	}
	if (TRInput::QInstance()->QKey(TRInputKey::RIGHT))
	{
		vTargetVelocity.x = 1;
	}

	if (vTargetVelocity.x == 0 && vTargetVelocity.y == 0)
		return;

	// We then need to normalize it
	float fLength = sqrtf(vTargetVelocity.x * vTargetVelocity.x + vTargetVelocity.y * vTargetVelocity.y);
	vTargetVelocity.x /= fLength;
	vTargetVelocity.y /= fLength;

	// Then multiply our by our speeds
	vTargetVelocity.x *= PLAYER_SPEED * FIXED_DELTA_TIME;
	vTargetVelocity.y *= PLAYER_SPEED * FIXED_DELTA_TIME;
}

/// <summary>
/// Handles any behavior for firing a shot and resetting the recovery timer
/// </summary>
void TRPlayer::HandleShotFired()
{
	TRWorld::QInstance()->InstanciateObject<TRProjectile>(ObjProjectile, *transform, 0.5f, CollisionLayer::CL_PLAYER_PROJECTILE)->InitializeProjectileData(10.0f, 0.0f, 1.0f);
	bCanShoot = false;
	iTicksSinceShot = 0;
}