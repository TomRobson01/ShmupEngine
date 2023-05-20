#include "TRWorldObject.h"

#include "DebugWindows/TRLoggerImGui.h"
#include "Rendering/TRRenderer.h"

TRWorldObject::TRWorldObject(TRObject& aBaseObj, Transform atInitialTransform)
{
	baseObject = aBaseObj;
	transform = atInitialTransform;
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
	Transform* t = QTransform();
	TRRenderer::QInstance().AddRenderTarget(
		t->QPositionX(),
		t->QPositionY(),
		t->QPositionZ(),
		t->QRotation(),
		baseObject.QTexture());
}

/// <summary>
/// Called a fixed number of times a frame.
/// </summary>
void TRWorldObject::OnFixedUpdate()
{
}
