#include "TRPlayer.h"

#include <iostream>

#include "TRInput.h"
#include "TRProjectile.h"
#include "TRWorldObject.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"

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

	const int iStartHealth = 5;
	int iHealth = iStartHealth;
	int iHealthHUDTexture = -1;
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
	fCollisionRadius = afColliderRadius;
	eCollisionLayer = aeLayer;
	objID = aiID;
	iHealth = 5;
	iHealthHUDTexture = TextureLoader::QInstance().RequestTexture("Assets/Textures/T_Heart.png");
	instancePtr = this;
}

TRPlayer::~TRPlayer()
{
}

void TRPlayer::OnStart()
{
	QTransform()->SetClamp(vPlayerXClamp.x, vPlayerXClamp.y, vPlayerYClamp.x, vPlayerYClamp.y);
	iHealth = 5;

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

	// Health UI
	for (int i = 0; i < iHealth; i++)
	{
		TRRenderer::QInstance().AddRenderTarget(
			(transform->QPositionX() - 1) + (0.5f * i),
			transform->QPositionY() - 0.75f,
			transform->QPositionZ(),
			transform->QRotation(),
			iHealthHUDTexture,
			0.2f);
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

	switch (apOtherObject->QCollisionLayer())
	{
	case CollisionLayer::CL_ENEMY_SUICIDER:
	case CollisionLayer::CL_ENEMY_PROJECTILE:
		TRWorld::QInstance()->RemoveWorldObject(apOtherObject->QID());
		TakeDamage(1);
		break;
	default:
		break;
	}
}

void TRPlayer::RestoreHealth(int aiHealAmount)
{
	TRLoggerImGui::QInstance()->AddLog("Player healed!", LogSeverity::TR_DEFAULT);
	iHealth += aiHealAmount;
	iHealth = std::min(iHealth, iStartHealth);
}

void TRPlayer::TakeDamage(int aiDamage)
{
	TRLoggerImGui::QInstance()->AddLog("Player hit!", LogSeverity::TR_DEFAULT);
	iHealth -= aiDamage;
	TRWorld::QInstance()->SetCamShakeDuration(0.5f);

	if (iHealth <= 0)
	{
		TRWorld::QInstance()->SetCamShakeDuration(2);
		Destroy(TRWorld::QInstance()->QObjExplosionPlayer());
		TRLoggerImGui::QInstance()->AddLog("Player died", LogSeverity::TR_ERROR);
		TRWorld::QInstance()->EndGame();
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

	// Tilt the ship based on the y velocity
	if (vTargetVelocity.y > 0)
	{
		transform->SetRotation(0.1f);
	}
	else if (vTargetVelocity.y < 0)
	{
		transform->SetRotation(-0.1f);
	}
	else
	{
		transform->SetRotation(0);
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
	TRWorld::QInstance()->InstanciateObject<TRProjectile>(TRWorld::QInstance()->QObjPlayerProjectile(), *transform, 0.5f, CollisionLayer::CL_PLAYER_PROJECTILE)->InitializeProjectileData(10.0f, 0.0f, 5.0f);
	bCanShoot = false;
	iTicksSinceShot = 0;
}