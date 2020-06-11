#pragma once
#include "SceneRunner.h"
#include <fstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

std::unique_ptr<Scene> SceneRunner::scene_m = nullptr;

SceneRunner::SceneRunner(const std::string& windowTitle, int width, int height, int samples, bool fullScreen) : debug_m(true), fullScreen_m(fullScreen)
{
	// Initialize GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	// Select OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	if (debug_m)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	if (samples > 0) {
		glfwWindowHint(GLFW_SAMPLES, samples);
	}

	if (fullScreen_m)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
		if (vmode)
		{
			window_m = glfwCreateWindow(vmode->width, vmode->height, windowTitle.c_str(), monitor, 0);
		}
	}
	else
	{
		window_m = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	}
	if (!window_m) {
		std::cerr << "Unable to create OpenGL context." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window_m);

	glfwSetCursorPosCallback(window_m, SceneRunner::onMouseMove);
	glfwSetMouseButtonCallback(window_m, SceneRunner::onMouseClick);
	glfwSetScrollCallback(window_m, SceneRunner::onMouseScroll);

	// Get framebuffer size
	glfwGetFramebufferSize(window_m, &fbw, &fbh);

	// Load the OpenGL functions.
	if (!gladLoadGL()) { exit(-1); }

	GLUtils::dumpGLInfo();

	imGuiInit();

	// Initialization
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	if (debug_m) {
		glDebugMessageCallback(GLUtils::debugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
			GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
	}

}

int SceneRunner::run()
{
	if (scene_m == nullptr)
	{
		std::cerr << "Run Scene Error: no scene is set!";
		return EXIT_FAILURE;
	}
	// Enter the main loop
	mainLoop(window_m);

	if (debug_m)
		glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
			GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");

	glfwTerminate();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return EXIT_SUCCESS;
}

std::string SceneRunner::parseCLArgs(int argc, char** argv, std::map<std::string, std::string>& sceneData)
{
	if (argc < 2) {
		printHelpInfo(argv[0], sceneData);
		exit(EXIT_FAILURE);
	}

	std::string recipeName = argv[1];
	auto it = sceneData.find(recipeName);
	if (it == sceneData.end()) {
		printf("Unknown recipe: %s\n\n", recipeName.c_str());
		printHelpInfo(argv[0], sceneData);
		exit(EXIT_FAILURE);
	}

	return recipeName;
}

void SceneRunner::setScene(std::unique_ptr<Scene> scene)
{
	SceneRunner::scene_m = std::move(scene);
}


void SceneRunner::printHelpInfo(const char* exeFile, std::map<std::string, std::string>& sceneData)
{
	printf("Usage: %s recipe-name\n\n", exeFile);
	printf("Recipe names: \n");
	for (auto it : sceneData) {
		printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
	}
}

void SceneRunner::mainLoop(GLFWwindow* window)
{

	SceneRunner::scene_m->setDimensions(fbw, fbh);
	SceneRunner::scene_m->initScene();
	SceneRunner::scene_m->resize(fbw, fbh);

	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);
		SceneRunner::scene_m->setFPS(calculateFPS());
		SceneRunner::scene_m->update(float(glfwGetTime()));
		SceneRunner::scene_m->render();
		glfwSwapBuffers(window);

		glfwPollEvents();
		int state = glfwGetKey(window, GLFW_KEY_SPACE);
		if (state == GLFW_PRESS)
			SceneRunner::scene_m->animate(!scene_m->animating());
	}
}

void SceneRunner::imGuiInit()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window_m, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

double SceneRunner::calculateFPS()
{
	static double prevSeconds = 0.0;
	static double frameCount = 0.0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - prevSeconds;

	if (elapsedSeconds > .025)
	{
		prevSeconds = currentSeconds;
		fps_m = frameCount / elapsedSeconds;
		// double perFrame = 1000.0 / fps_m;
		frameCount = 0;
	}

	frameCount++;

	return fps_m;
}

void SceneRunner::onMouseMove(GLFWwindow* window, double posX, double posY)
{
	if (SceneRunner::scene_m == nullptr)
		return;

	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		SceneRunner::scene_m->mouseMove(MouseEvent(x, y, false, true));
	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		SceneRunner::scene_m->mouseMove(MouseEvent(x, y, true, false));
	}

	SceneRunner::scene_m->mouseMove(MouseEvent(x, y, false, false));
}

void SceneRunner::onMouseClick(GLFWwindow* window, int mouseButtn, int action, int mods)
{
	if (SceneRunner::scene_m == nullptr)
		return;

	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
		return;

	if (mouseButtn == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		SceneRunner::scene_m->mouseDown(MouseEvent(static_cast<float>(x), static_cast<float>(y), true, false));
	}
	else if (mouseButtn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		SceneRunner::scene_m->mouseDown(MouseEvent(static_cast<float>(x), static_cast<float>(y), false, true));

	}
}

void SceneRunner::onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	// for most mouses, deltaY has the value
	SceneRunner::scene_m->mouseScroll(deltaX, deltaY);
}
