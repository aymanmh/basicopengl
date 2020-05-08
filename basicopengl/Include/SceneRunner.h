#pragma once
#include <glad/glad.h>
#include "Scene.h"
#include <GLFW/glfw3.h>
#include "glutils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <iostream>
#include <memory>

class SceneRunner {
private:
    GLFWwindow* window_m;
    int fbw, fbh;
    bool debug_m;           // Set true to enable debug messages

public:
    SceneRunner(const std::string& windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0);

    int run(std::unique_ptr<Scene> scene);

    static std::string parseCLArgs(int argc, char** argv, std::map<std::string, std::string>& sceneData);

private:
    static void printHelpInfo(const char* exeFile, std::map<std::string, std::string>& sceneData);

    void mainLoop(GLFWwindow* window, std::unique_ptr<Scene> scene);
};