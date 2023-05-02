#pragma once
#include "Physics/TRPhysics.h"

class TRObject
{
public:
	TRObject() = default;
	TRObject(const char* asName, const char* asTextureName, Transform atInitialTransform);
	~TRObject();

	void Update();

	Transform* QTransform() { return &transform; }
	unsigned int QTexture() { return texture; }


private:
	Transform transform;

	const char* name;
	unsigned int texture;

};

