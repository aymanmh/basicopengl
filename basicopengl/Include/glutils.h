#pragma once
#pragma once

// Copyright(c) 2017 Packt
// MIT License (Check Licenses/Licenses.mit for full text)

#include <glad/glad.h>

namespace GLUtils
{
    int checkForOpenGLError(const char*, int);

    void dumpGLInfo(bool dumpExtensions = false);

    void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar* msg, const void* param);
}