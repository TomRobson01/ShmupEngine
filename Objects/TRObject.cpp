#include "TRObject.h"

#include "Rendering/TextureLoader.h"

#include <iostream>
#include <string>

TRObject::TRObject(const char* asName, const char* asTextureName)
{
	name = asName;

	// Once we have everything ready, request our texture from the loader
	vAnimTextures.push_back(TextureLoader::QInstance().RequestTexture(asTextureName));
}

TRObject::TRObject(const char* asName, const char* asTextures[])
{
	const int iTexturesToRegister = sizeof(asTextures) / sizeof(const char*);
	for (int i = 0; i <= iTexturesToRegister; i++)
	{
		vAnimTextures.push_back(TextureLoader::QInstance().RequestTexture(asTextures[i]));
	}
}

TRObject::~TRObject()
{
}