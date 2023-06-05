#include "TRWorld.h"
#include <iostream>

#include "DebugWindows/TRLoggerImGui.h"
#include "Rendering/TRRenderer.h"

TRWorldObject::TRWorldObject(TRObject& aBaseObj, Transform atInitialTransform, int aiID)
{
	baseObject = aBaseObj;
	transform = new Transform();
	transform->Translate(atInitialTransform.QPositionX(), atInitialTransform.QPositionY(), atInitialTransform.QPositionZ(), atInitialTransform.QRotation());
	objID = aiID;
}

TRWorldObject::~TRWorldObject()
{
}

/// <summary>
/// Called when the object is first instanciated.
/// </summary>
void TRWorldObject::OnStart()
{
	TRLoggerImGui::QInstance()->AddLog("New object started!", LogSeverity::TR_DEFAULT);
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
}

/// <summary>
/// Called a fixed number of times a frame.
/// </summary>
void TRWorldObject::OnFixedUpdate()
{
}

void TRWorldObject::Destroy()
{
	TRWorld::QInstance()->RemoveWorldObject(QID());
}
