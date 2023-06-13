#include "TREnemy.h"

#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRProjectile.h"
#include "Objects/TRWorld.h"

namespace
{
	ImVec2 vWantDir		= ImVec2(-1, 0);
	float fMoveSpeed	= 1.0f;
	float fHealth		= 10.0f;

	TRPlayer* pPlayer;

	TRObject ObjProjectile;
	ImVec2 vFireCooldownRange = ImVec2(4, 10);
	float fCurrentFireCooldown = 0.0f;
	float fTimeSinceFire = 0.0f;
}

TREnemy::TREnemy(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	collider = new CircleCollider(afColliderRadius, aeLayer);
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
	ObjProjectile = TRObject("Projectile", "Assets/Textures/T_Temp_Player_Sprite.png");
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
	// Collision checks
	TRPhysics::QInstance()->QIsCollidingWithAnyInLayer(this, CollisionLayer::CL_PLAYER);
	TRPhysics::QInstance()->QIsCollidingWithAnyInLayer(this, CollisionLayer::CL_PLAYER_PROJECTILE);

	// Tick projectile behavior
	if (fTimeSinceFire > fCurrentFireCooldown)
	{
		FireShot();
	}
	else
	{
		fTimeSinceFire += FIXED_DELTA_TIME;
	}

	this->TRWorldObject::OnFixedUpdate();
}

/// <summary>
/// Called whenever this object collides with another
/// </summary>
/// <param name="apOtherObject">The other object we collided with</param>
void TREnemy::OnCollision(TRWorldObject* apOtherObject)
{
	switch (apOtherObject->QCollider()->QCollisionLayer())
	{
	case CollisionLayer::CL_PLAYER_PROJECTILE:
		// Destroy the projectile, and tick health
		TRWorld::QInstance()->RemoveWorldObject(apOtherObject->QID());
		DamageHealth(2);
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
		TRWorld::QInstance()->RemoveWorldObject(QID());
	}
}

/// <summary>
/// Fire a projectile, and reset the cooldown timer.
/// </summary>
void TREnemy::FireShot()
{
	TRWorld::QInstance()->InstanciateObject<TRProjectile>(ObjProjectile, *transform, 0.5f, CollisionLayer::CL_ENEMY_PROJECTILE)->InitializeProjectileData(-10.0f, 0.0f, 1.0f);
	fTimeSinceFire = 0;
	float t = rand() % 1;
	fCurrentFireCooldown = vFireCooldownRange.x + t * (vFireCooldownRange.y, vFireCooldownRange.x);
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
		// Progressing left
		vWantDir = ImVec2(-1, 0);
		break;
	case ENEMY_MOTION_TYPE::MT_HOMING:
		// Progressing towards the player
		if (pPlayer)
		{
			ImVec2 playerPos = ImVec2(pPlayer->QTransform()->QPositionX(), pPlayer->QTransform()->QPositionY());
			ImVec2 myPos = ImVec2(QTransform()->QPositionX(), QTransform()->QPositionY());

			ImVec2 vDelta = ImVec2(playerPos.x - myPos.x, playerPos.y - myPos.y);

			// UGLY VECTOR CALCULATION!
			// We need to make a dedicated vector library at some point...
			float fTangent = abs(vDelta.y);
			float fAdjacent = abs(vDelta.x);
			float fDist = sqrtf((fTangent * fTangent) + (fAdjacent * fAdjacent));

			vWantDir = ImVec2(vDelta.x / fDist, vDelta.y / fDist);
		}
		break;
	default:
		break;
	}

	// Translate based on our wantDir
	QTransform()->Translate(vWantDir.x* fMoveDelta, vWantDir.y* fMoveDelta, 0, 0);
}
