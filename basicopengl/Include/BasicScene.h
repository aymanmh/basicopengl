#pragma once
#include "scene.h"

#include <GLAD/glad.h>
#include <string>

class SceneBasic : public Scene
{
private:
    GLuint vaoHandle;
    GLuint programHandle;

    void linkMe(GLint vertShader, GLint fragShader);
    void compileShaderProgram();
    void writeShaderBinary();
    void loadShaderBinary(GLint);
    void loadSpirvShader();

    std::string getShaderInfoLog(GLuint shader);
    std::string getProgramInfoLog(GLuint program);

public:
    SceneBasic();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

};