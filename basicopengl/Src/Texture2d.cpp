#include "Texture2d.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "glutils.h"

Texture2d::Texture2d() : texture_m(0)
{
}

Texture2d::~Texture2d()
{
}

bool Texture2d::loadTexture(const std::string & filename, bool genMipMaps)
{
	int width, height, components;
	unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
	if (imageData == nullptr)
	{
		std::cerr << "error loading file " << filename << std::endl;
		return false;
	}

	glGenTextures(1, &texture_m);
	glBindTexture(GL_TEXTURE_2D, texture_m);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
//	glTextureParameteri(texture_m, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTextureParameteri(texture_m, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (genMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	stbi_image_free(imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

void Texture2d::bind(GLuint textUnit)
{
	glActiveTexture(GL_TEXTURE0 + textUnit);
	glBindTexture(GL_TEXTURE_2D, texture_m);
}
