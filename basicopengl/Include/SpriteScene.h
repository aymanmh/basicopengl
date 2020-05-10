#pragma once
#include "scene.h"
#include "GLSLProgram.h"
#include <GLAD/glad.h>
#include <string>
#include <memory>
#include <vector>
#include "Texture2d.h"

class Quad;
class MyPoint;

class SpriteScene : public Scene
{
private:
    GLuint vaoHandle_m;
    GLSLProgram program_m;
    GLSLProgram basicProgram_m;
    std::vector<MyPoint> myPoints_m;
    Texture2d flair_m;
    Texture2d space_m;
    GLuint hdrFbo_m, blurFbo_m;
    std::unique_ptr<Quad> quad_m;
    GLuint renderTex_m;
    bool switchImage = false;
    float lastTime = 0.0f;
    void compileShaderProgram();
    void setupFBO();
    GLuint spirtesVbo;
public:
    SpriteScene();

    void initScene() override;
    void update(float t) override;
    void render() override;
    void resize(int, int) override;
    void mouseDown(MouseEvent event) override;

    void renderToTexture();
    void drawScene();
    void drawFBO();
};