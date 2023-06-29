#pragma once

#include <map>
#include <string>
#include <vector>


// The texture loader is responsible for
//    - Loading all textures into memory when requested
//    - Dumping textures from texture memory when needed
//    - Maintianing a map of all currently loaded textures, so that we only need a single instance of a texture loaded

class TextureLoader
{
public:
	static TextureLoader& const QInstance()
	{
		static TextureLoader instance;
		return instance;
	};

	unsigned int RequestTexture(std::string asTexturePath);

protected:
	unsigned int LoadTexture(std::string asTexturePath);

private:
	std::map<std::string, unsigned int> TextureHeap;
};

