#pragma once
#include "GLAD/glad.h"
#include <string>
class Texture2d
{
public:
	Texture2d();
	~Texture2d();
	bool loadTexture(const std::string& filename, bool genMipMaps = true);
	void bind(GLuint textUnit = 0);

private:
	GLuint texture_m;
};

