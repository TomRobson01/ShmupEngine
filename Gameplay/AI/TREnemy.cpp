#include "TREnemy.h"
#include "TRMath.h"
#include "TRWaveManager.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRProjectile.h"
#include "Objects/TRWorld.h"

#include <iostream>

namespace
{
	ImVec2 vWantDir	= ImVec2(-1, 0);
	ImVec2 vFireCooldownRange = ImVec2(4, 10);
	TRPlayer* pPlayer;
}

TREnemy::TREnemy(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	fCollisionRadius = afColliderRadius;
	eCollisionLayer = aeLayer;
	objID = aiID;

	motionType = ENEMY_MOTION_TYPE::MT_PROGRESS;
	pPlayer = TRPlayer::QInstance();
}

TREnemy::~TREnemy()
{
}

/// <summary>
/// Called when the enemy is first initialized
/// </summary>
void TREnemy::OnStart()
{
	TRLoggerImGui::QInstance()->AddLog("New enemy started!", LogSeverity::TR_DEFAULT);
	this->TRWorldObject::OnStart();
}

/// <summary>
/// Called every frame
/// </summary>
void TREnemy::OnUpdate()
{
	//Move the enemy
	TickMotion();
	this->TRWorldObject::OnUpdate();
}

/// <summary>
/// Called at fixed intgervals - usually 60 times a frame
/// </summary>
void TREnemy::OnFixedUpdate()
{
	// Tick projectile behavior
	if (fTimeSinceFire > fCurrentFireCooldown)
	{
		FireShot();
	}
	else
	{
		fTimeSinceFire += FIXED_DELTA_TIME;
	}

	fTime += FIXED_DELTA_TIME;

	if (transform->QPositionX() <= -25)
	{
		// Off the end, despawn
		TRWaveManager::QInstance()->OnEnemyDeath(true);
		Destroy();
	}

	if (TRWorld::QInstance()->QGameEnded())
	{
		Destroy(TRWorld::QInstance()->QObjExplosionEnemy());
	}

	this->TRWorldObject::OnFixedUpdate();
}


bool bDone = false;
/// <summary>
/// Called whenever this object collides with another
/// </summary>
/// <param name="apOtherObject">The other object we collided with</param>
void TREnemy::OnCollision(TRWorldObject* apOtherObject)
{
	if (!apOtherObject)
		return;

	switch (apOtherObject->QCollisionLayer())
	{
	case CollisionLayer::CL_PLAYER_PROJECTILE:
		// Destroy the projectile, and tick health
		TRWorld::QInstance()->RemoveWorldObject(apOtherObject->QID());
		DamageHealth(2);
		break;
	case CollisionLayer::CL_PLAYER:
		if (apOtherObject->QCollisionLayer() == CollisionLayer::CL_ENEMY_SUICIDER)
			TRWaveManager::QInstance()->OnEnemyDeath();
			Destroy(TRWorld::QInstance()->QObjExplosionEnemy());
		break;
	default:
		break;
	}
}

/// <summary>
/// Reduce the health by a set float, and if it reaches 0, kill this enemy.
/// </summary>
/// <param name="afDamage">The amount of damage to take.</param>
void TREnemy::DamageHealth(float afDamage)
{
	fHealth -= afDamage;
	if (fHealth <= 0)
	{
		TRLoggerImGui::QInstance()->AddLog("Enemy died", LogSeverity::TR_DEFAULT);
		TRWaveManager::QInstance()->OnEnemyDeath();
		TRWaveManager::QInstance()->TrySpawnDrop(QTransform()->QPositionX(), QTransform()->QPositionY());
		Destroy(TRWorld::QInstance()->QObjExplosionEnemy());
	}
}

/// <summary>
/// Fire projectiles from each slot, and reset the cooldown timer.
/// </summary>
void TREnemy::FireShot()
{
	for (std::tuple<float, float> pos : vFirePoints)
	{
		Transform t = *transform;
		t.Translate(std::get<0>(pos), std::get<1>(pos), 0, 0);
		TRWorld::QInstance()->InstanciateObject<TRProjectile>(TRWorld::QInstance()->QObjEnemyProjectile(), t, 0.5f, CollisionLayer::CL_ENEMY_PROJECTILE)->InitializeProjectileData(-5.5f, 0.0f, 5.0f);
	}
		
	fTimeSinceFire = 0;
	float t = rand() % 1;
	fCurrentFireCooldown = vFireCooldownRange.x + t * (vFireCooldownRange.y - vFireCooldownRange.x);
}

/// <summary>
/// Handle any motion and translation for this frame.
/// </summary>
void TREnemy::TickMotion()
{
	float fMoveDelta = fMoveSpeed * FIXED_DELTA_TIME;

	// Calculate vWantDir
	switch (motionType)
	{
	case ENEMY_MOTION_TYPE::MT_PROGRESS:
		vWantDir = ImVec2(-1, 0);
		break;
	case ENEMY_MOTION_TYPE::MT_SINE:
	{
		float fYOffset = TRMath::FSine(1.0f, 0.1f, fTime);
		vWantDir = ImVec2(-1, fYOffset);
		break;
	}
	case ENEMY_MOTION_TYPE::MT_HOMING:
		// Progressing towards the player
		if (pPlayer)
		{
			ImVec2 playerPos = ImVec2(pPlayer->QTransform()->QPositionX(), pPlayer->QTransform()->QPositionY());
			ImVec2 myPos = ImVec2(QTransform()->QPositionX(), QTransform()->QPositionY());

			ImVec2 vDelta = ImVec2(playerPos.x - myPos.x, playerPos.y - myPos.y);
			TRMath::VFNormalize(vDelta.x, vDelta.y);

			vWantDir = vDelta;
		}
		break;
	default:
		break;
	}

	// Translate based on our wantDir
	QTransform()->Translate(vWantDir.x* fMoveDelta, vWantDir.y* fMoveDelta, 0, 0);
}
