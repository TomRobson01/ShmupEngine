#pragma once

/// <summary>
/// TRObject acts as the template from which we will instanciate new WorldObjects.
/// </summary>
class TRObject
{
public:
	TRObject() = default;
	TRObject(const char* asName, const char* asTextureName);
	~TRObject();

	unsigned int QTexture() { return texture; }


private:
	const char* name;
	unsigned int texture;

};

