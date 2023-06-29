#include "TRWorld.h"

#include <iostream>

#include "TRDefines.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Rendering/TRRenderer.h"

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

/// <summary>
/// Informs this TRWorldObject that it is colliding with another object. THREAD SAFE.
/// </summary>
/// <param name="aiCollidingObjectID">The ID of the colliding object, to grab the TRWorldObject* on the main thread</param>
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
		baseObject.QTexture(uiCurrentAnimIndex));

	collider->SetPosition(transform->QPositionX(), transform->QPositionY());

}

/// <summary>
/// Called a fixed number of times a frame.
/// </summary>
void TRWorldObject::OnFixedUpdate()
{
	// Check if enough time has elapsed since the last animation update
	const clock_t cCurrentClock = clock();
	float fTimeSinceLastAnimationUpdate = cCurrentClock - lastAnimationUpdate;
	if (fTimeSinceLastAnimationUpdate > (60 / ANIMATION_FPS) * FIXED_UPDATE_TICKS)
	{
		OnAnimationUpdate();
		lastAnimationUpdate = cCurrentClock;
	}

	// Check if we have an unresolved collision
	if (iCollidingObjectID > -1)
	{
		OnCollision(TRWorld::QInstance()->GetWorldObjectByID(iCollidingObjectID).get()); 
		iCollidingObjectID = -1;
	}
}

/// <summary>
/// Called OnFixedUpdate when there is currently an object colliding with the object.
/// </summary>
/// <param name="apOtherObject">A pointer to the colliding TRWorldObject</param>
void TRWorldObject::OnCollision(TRWorldObject* apOtherObject)
{
	if (!apOtherObject)
		return;
}

/// <summary>
/// Called a fixed number of times a second, and progresses the animation loop to the next frame.
/// </summary>
void TRWorldObject::OnAnimationUpdate()
{
	uiCurrentAnimIndex++;
	if (uiCurrentAnimIndex >= baseObject.QTexturesLoaded())
	{
		uiCurrentAnimIndex = 0;
	}
}

/// <summary>
/// Removes the object from the world.
/// </summary>
void TRWorldObject::Destroy()
{
	TRWorld::QInstance()->RemoveWorldObject(QID());
}
