#pragma once

#include <vector>

/// <summary>
/// TRObject acts as the template from which we will instanciate new WorldObjects.
/// </summary>
class TRObject
{
public:
	TRObject() = default;
	TRObject(const char* asName, const char* asTextureName);
	TRObject(const char* asName, const char* asTextures[]);
	~TRObject();

	unsigned int const QTexture(unsigned int aiIndex = 0) { return aiIndex < vAnimTextures.size() ? vAnimTextures[aiIndex] : -1; }
	unsigned int const QTexturesLoaded()					{ return vAnimTextures.size(); }

	const char* const QName() { return name; }

private:
	const char* name;

	std::vector<unsigned int> vAnimTextures;

};

