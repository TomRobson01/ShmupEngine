#include "TRDrop.h"

#include "TRMath.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRPlayer.h"
#include "Objects/TRProjectile.h"
#include "Objects/TRWorld.h"

#include <iostream>

namespace
{
	ImVec2 vWantDir;
	TRPlayer* pPlayer;
}

TRDrop::TRDrop(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	fCollisionRadius = afColliderRadius;
	eCollisionLayer = aeLayer;
	objID = aiID;

	dropType = DROP_TYPE::DROP_HEART;
	pPlayer = TRPlayer::QInstance();
}

TRDrop::~TRDrop()
{
}

void TRDrop::OnUpdate()
{
	const float fMoveDelta = fMoveSpeed * FIXED_DELTA_TIME;

	// Move towards the player
	if (pPlayer)
	{
		ImVec2 playerPos = ImVec2(pPlayer->QTransform()->QPositionX(), pPlayer->QTransform()->QPositionY());
		ImVec2 myPos = ImVec2(QTransform()->QPositionX(), QTransform()->QPositionY());

		ImVec2 vDelta = ImVec2(playerPos.x - myPos.x, playerPos.y - myPos.y);
		TRMath::VFNormalize(vDelta.x, vDelta.y);

		vWantDir = vDelta;
		QTransform()->Translate(vWantDir.x * fMoveDelta, vWantDir.y * fMoveDelta, 0, 0);


		// Best to do the dist check here, as we've alredy calculated it. No point adding another collision layer for these
		if (TRMath::FDist(myPos.x, myPos.y, playerPos.x, playerPos.y) < pPlayer->QCollisionRadius() * 1.5f)
		{
			TRLoggerImGui::QInstance()->AddLog("Got pickup", LogSeverity::TR_DEFAULT);
			TRPlayer::QInstance()->RestoreHealth(1);
			Destroy(TRWorld::QInstance()->QObjPickupHeart());
		}
	}
	this->TRWorldObject::OnUpdate();
}