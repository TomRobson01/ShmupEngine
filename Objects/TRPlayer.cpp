#include "TRPlayer.h"

#include <iostream>

#include "TRInput.h"
#include "DebugWindows/TRLoggerImGui.h"
#include "Objects/TRWorld.h"

#define PLAYER_SPEED 5	// We'll want this to be exposed to the editor eventually, but until we know what form that takes, let's just define it here

namespace
{
	ImVec2 vCurrentVelocity;	// Our current velocity - interpolates towards vTargetVelocity per fixed update
	ImVec2 vTargetVelocity;		// Our "wish" velocity

	const ImVec2 vPlayerXClamp = ImVec2(-15.5f, -2.0f);
	const ImVec2 vPlayerYClamp = ImVec2(-11.5f, 11.5f);
}

TRPlayer* TRPlayer::instancePtr;
TRPlayer* TRPlayer::QInstance()
{
	if (instancePtr == nullptr)
	{
		instancePtr = new TRPlayer();
	}
	return instancePtr;
}

TRPlayer::TRPlayer(TRObject& aBaseObj, Transform atInitialTransform)
{
	baseObject = aBaseObj;
	transform = atInitialTransform;
}

TRPlayer::~TRPlayer()
{
}

void TRPlayer::OnStart()
{
	TRLoggerImGui::QInstance()->AddLog("New player started!", LogSeverity::TR_DEFAULT);
	transform.SetClamp(vPlayerXClamp.x, vPlayerXClamp.y, vPlayerYClamp.x, vPlayerYClamp.y);
	this->TRWorldObject::OnStart();
}

void TRPlayer::OnUpdate()
{
	CalculateTargetVelocity();
	transform.Translate(vTargetVelocity.x, vTargetVelocity.y, 0, 0);
	this->TRWorldObject::OnUpdate();
}

void TRPlayer::OnFixedUpdate()
{
	std::string outStr = "Player position: " + std::to_string(transform.QPositionX()) + ", " + std::to_string(transform.QPositionY());
	std::cout << outStr << "\n";
	this->TRWorldObject::OnFixedUpdate();
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