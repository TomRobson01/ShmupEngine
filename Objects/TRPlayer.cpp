#include "TRPlayer.h"

#include <iostream>

#include "TRInput.h"
#include "TRProjectile.h"
#include "TRWorldObject.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Interface/TRInterface.h"
#include "Interface/TRUIElement.h"
#include "Objects/TRWorld.h"
#include "Rendering/TextureLoader.h"

#define PLAYER_SPEED 5	// We'll want this to be exposed to the editor eventually, but until we know what form that takes, let's just define it here
#define CLOCKS_BETWEEN_SHOTS 5

namespace
{
	ImVec2 vCurrentVelocity;	// Our current velocity - interpolates towards vTargetVelocity per fixed update
	ImVec2 vTargetVelocity;		// Our "wish" velocity

	constexpr ImVec2 vPlayerXClamp = ImVec2(-15.5f, -2.0f);
	constexpr ImVec2 vPlayerYClamp = ImVec2(-11.5f, 11.5f);

	int iTicksSinceShot		= 0;
	bool bCanShoot			= false;

	constexpr int iStartHealth = 5;
	int iHealth = iStartHealth;
	int iHealthHUDTexture = -1;

	float fTricannonTime = 0.0f;
	constexpr float fTricannonDuration = 10.f;
	ImVec2 vShotPositionsA[3] = { ImVec2(0, 0), ImVec2(0, 1), ImVec2(0, -1) };

	TRUIElement* pHeartsUI[5] = { nullptr };

	void UpdateHeartsUI()
	{
		for (int i = 0; i < 5; ++i)
		{
			if (pHeartsUI[i])
			{
				pHeartsUI[i]->SetVisibility(i < iHealth);
			}
		}
	}
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
	/*for (int i = 0; i < 5; ++i)
	{
		if (pHeartsUI[i])
		{
			delete pHeartsUI[i];
		}
	}*/
}

void TRPlayer::OnStart()
{
	QTransform()->SetClamp(vPlayerXClamp.x, vPlayerXClamp.y, vPlayerYClamp.x, vPlayerYClamp.y);
	iHealth = 5;

	// Create hearts UI
	for (int i = 0; i < iHealth; ++i)
	{
		if (!pHeartsUI[i])
		{
			pHeartsUI[i] = new TRUIElement(0.05f + (0.05f * i), 0.05f, 0.5f, iHealthHUDTexture);
			TRInterface::QInstance()->AddUIElement(pHeartsUI[i]);
		}
		else
		{
			pHeartsUI[i]->SetVisibility(true);
		}
	}

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

	if (bTriCannonActive)
	{
		fTricannonTime -= FIXED_DELTA_TIME;
		bTriCannonActive = fTricannonTime > 0;
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
	UpdateHeartsUI();
}

void TRPlayer::TakeDamage(int aiDamage)
{
	TRLoggerImGui::QInstance()->AddLog("Player hit!", LogSeverity::TR_DEFAULT);
	iHealth -= aiDamage;
	TRWorld::QInstance()->SetCamShakeDuration(0.5f);
	UpdateHeartsUI();

	if (iHealth <= 0)
	{
		TRWorld::QInstance()->SetCamShakeDuration(2);
		Destroy(TRWorld::QInstance()->QObjExplosion());
		TRLoggerImGui::QInstance()->AddLog("Player died", LogSeverity::TR_ERROR);
		TRWorld::QInstance()->EndGame();
	}
}

/// <summary>
/// If not already active, enables the tricannon
/// </summary>
void TRPlayer::RequestTricannon()
{
	if (!bTriCannonActive)
	{
		bTriCannonActive = true;
		fTricannonTime = fTricannonDuration;
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
	int iShotsToFire = 1;
	if (bTriCannonActive)
	{
		iShotsToFire = 3;
	}

	for (int i = 0; i < iShotsToFire; ++i)
	{
		// Construct the fire point
		Transform tFirePos = *transform;
		tFirePos.Translate(vShotPositionsA[i].x, vShotPositionsA[i].y, 0, 0);

		TRWorld::QInstance()->InstanciateObject<TRProjectile>(TRWorld::QInstance()->QObjPlayerProjectile(), tFirePos, 0.5f, CollisionLayer::CL_PLAYER_PROJECTILE)->InitializeProjectileData(10.0f, 0.0f, 5.0f);
	}
	bCanShoot = false;
	iTicksSinceShot = 0;
}