#include "TRObject.h"
#include <iostream>
#include "Rendering/TextureLoader.h"

TRObject::TRObject(const char* asName, const char* asTextureName, Transform atInitialTransform)
{
	name = asName;
	transform = atInitialTransform;

	// Once we have everything ready, request our texture from the loader
	texture = TextureLoader::QInstance().RequestTexture(asTextureName);
}

TRObject::~TRObject()
{
}

void TRObject::Update()
{
	// Gameplay tick here
	// QTransform()->Rotate(0.01f);
}