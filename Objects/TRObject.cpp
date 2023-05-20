#include "TRObject.h"
#include <iostream>
#include "Rendering/TextureLoader.h"

TRObject::TRObject(const char* asName, const char* asTextureName)
{
	name = asName;

	// Once we have everything ready, request our texture from the loader
	texture = TextureLoader::QInstance().RequestTexture(asTextureName);
}

TRObject::~TRObject()
{
}