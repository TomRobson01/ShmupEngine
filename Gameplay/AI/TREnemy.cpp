#include "TREnemy.h"

#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRWorld.h"

namespace
{
	ImVec2 vWantDir		= ImVec2(-1, 0);
	float fMoveSpeed	= 2.0f;
	float fHealth		= 10.0f;
}

TREnemy::TREnemy(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	collider = new CircleCollider(afColliderRadius, aeLayer);
	objID = aiID;
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
	// We probably want a proper function to calculate want dir toaccomodate for wave functions and such
	float fMoveDelta = fMoveSpeed * FIXED_DELTA_TIME;
	//QTransform()->Translate(vWantDir.x * fMoveDelta, vWantDir.y * fMoveDelta, 0, 0);

	this->TRWorldObject::OnUpdate();
}

/// <summary>
/// Called at fixed intgervals - usually 60 times a frame
/// </summary>
void TREnemy::OnFixedUpdate()
{
	// Debug - Collision checks with the player here to check the collision checks are working
	//if (TRPlayer::QInstance() && TRPlayer::QInstance()->QCollider() && TRPhysics::QInstance()->QIsColliding(*collider, *TRPlayer::QInstance()->QCollider()))
	TRPhysics::QInstance()->QIsCollidingWithAnyInLayer(this, CollisionLayer::CL_PLAYER);
	TRPhysics::QInstance()->QIsCollidingWithAnyInLayer(this, CollisionLayer::CL_PLAYER_PROJECTILE);
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
		TRWorld::QInstance()->RemoveWorldObject(apOtherObject->QID());
		break;
	default:
		break;
	}
}