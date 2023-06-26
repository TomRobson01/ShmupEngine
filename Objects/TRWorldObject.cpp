#include "TRWorld.h"

#include <iostream>
#include <mutex>

#include "DebugWindows/TRLoggerImGui.h"
#include "Rendering/TRRenderer.h"

namespace 
{
	std::mutex m;
};

TRWorldObject::TRWorldObject(TRObject& aBaseObj, Transform atInitialTransform, float afColliderRadius, CollisionLayer aeLayer, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	collider = new CircleCollider(afColliderRadius, aeLayer);
	objID = aiID;
}

TRWorldObject::~TRWorldObject()
{
	TRPhysics::QInstance()->UnRegisterCollider(this);
}

void TRWorldObject::CallOnCollision(int aiCollidingObjectID)
{
	iCollidingObjectID = aiCollidingObjectID;
}

/// <summary>
/// Called when the object is first instanciated.
/// </summary>
void TRWorldObject::OnStart()
{
	//TRLoggerImGui::QInstance()->AddLog("New object started!", LogSeverity::TR_DEFAULT);
}

/// <summary>
/// Called every frame.
/// </summary>
void TRWorldObject::OnUpdate()
{
	TRRenderer::QInstance().AddRenderTarget(
		transform->QPositionX(),
		transform->QPositionY(),
		transform->QPositionZ(),
		transform->QRotation(),
		baseObject.QTexture());

	collider->SetPosition(transform->QPositionX(), transform->QPositionY());
}

/// <summary>
/// Called a fixed number of times a frame.
/// </summary>
void TRWorldObject::OnFixedUpdate()
{
	if (iCollidingObjectID > -1)
	{
		OnCollision(TRWorld::QInstance()->GetWorldObjectByID(iCollidingObjectID).get()); 
		iCollidingObjectID = -1;
	}
}

void TRWorldObject::OnCollision(TRWorldObject* apOtherObject)
{
	if (!apOtherObject)
		return;
}

void TRWorldObject::Destroy()
{
	TRWorld::QInstance()->RemoveWorldObject(QID());
}
