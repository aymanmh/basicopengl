#pragma once
#include "scene.h"
#include "GLSLProgram.h"

#include <GLAD/glad.h>
#include <string>

using namespace std;

class BasicScene : public Scene
{
private:
    GLuint vaoHandle_m;
    GLSLProgram program_m;

    void compileShaderProgram();

public:
    BasicScene();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

};