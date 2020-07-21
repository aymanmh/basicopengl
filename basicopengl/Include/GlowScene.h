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

class GlowScene : public Scene
{
private:
    GLuint vaoHandle_m;
    GLSLProgram program_m;
    GLSLProgram basicProgram_m;
    GLSLProgram blurProgram_m;
    std::vector<MyPoint> myPoints_m;
    Texture2d flair_m;
    Texture2d space_m;
    GLuint hdrFbo_m = 0;
    GLuint blurFBO_m = 0;
    std::unique_ptr<Quad> quad_m;
    GLuint hdrTexture_m, brightnessTexture_m, blurTexture1_m, blurTexture2_m;
    bool ShowHDR_m = false;
    bool ShowBrightness_m = false;
    bool ShowBlur_m = false;
    bool bloom_m = true;
    float lastTime = 0.0f;
    float luminanceThresh_m = 0.1f;
    float exposure_m = 1.0f;
    float gamma_m = 2.2f;
    float sigma_m = 8.0f;
    float weights_m[5] = { 0.0f };
    void compileShaderProgram();
    void setupFBO();
    void hBlur();
    void vBlur();

    GLuint spirtesVbo_m;
    glm::mat4  mvp_m;


public:
    GlowScene();

    void initScene() override;
    void update(float t) override;
    void render() override;
    void resize(int, int) override;
    void mouseDown(MouseEvent event) override;

private:
    void renderToTexture();
    void drawScene(int pass);
    void drawFBO();
    void drawIMGUI();
    float gaussian(float x, float sigma2);
    void computeGaussianWeights();
    void finalDraw();
};