#include "TextureLoader.h"

#include <iostream>	// Temp iostream include, replace with our proper debugger later

#define STB_IMAGE_IMPLEMENTATION
#include "stb-image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// <summary>
/// Request a texture from the heap. If it doesn't exist, load it for the first time.
/// </summary>
/// <param name="asTexturePath">Path where the desired texture will be loaded from.</param>
/// <returns>Unsigned int containing the requested textures data.</returns>
unsigned int TextureLoader::RequestTexture(std::string asTexturePath)
{
	// We only want to load a new texture if it doesn't exist in the texture heap already
	if (TextureHeap.find(asTexturePath) != TextureHeap.end())
	{
		return TextureHeap.at(asTexturePath);
	}
	else
	{
		return LoadTexture(asTexturePath);
	}
}

/// <summary>
/// Loads the requested texture into memory, and caches it on the TextureHeap map for future use.
/// </summary>
/// <param name="asTexturePath">Path where the desired texture will be loaded from.</param>
/// <returns>Unsigned int containing the requested textures data.</returns>
unsigned int TextureLoader::LoadTexture(std::string asTexturePath)
{
	unsigned int iTexture;

	glGenTextures(1, &iTexture);
	glBindTexture(GL_TEXTURE_2D, iTexture);	// Until a new texture is bound, all GL_TEXTURE_2D operations now affect this texture

	// Set the wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load image, create texture, and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load(asTexturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << asTexturePath << std::endl;
	}
	stbi_image_free(data);
	TextureHeap.insert({ asTexturePath, iTexture });

	return iTexture;
}
