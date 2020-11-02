#pragma once
// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)
#include "GLAD/glad.h"

namespace NoiseTexture
{
	int storeTex(GLfloat* data, int w, int h);

	int generate2DTex(float baseFreq = 4.0f, float persistence = 0.5f, int w = 128, int h = 128, bool periodic = false);

	int generatePeriodic2DTex(float baseFreq = 4.0f, float persist = 0.5f, int w = 128, int h = 128);
};