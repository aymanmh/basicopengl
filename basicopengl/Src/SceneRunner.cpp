#pragma once
#include "SceneRunner.h"
#include <fstream>

SceneRunner::SceneRunner(const std::string& windowTitle, int width, int height , int samples) : debug_m(true)
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

	// Open the window
	window_m = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	if (!window_m) {
		std::cerr << "Unable to create OpenGL context." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window_m);

	// Get framebuffer size
	glfwGetFramebufferSize(window_m, &fbw, &fbh);

	// Load the OpenGL functions.
	if (!gladLoadGL()) { exit(-1); }

	GLUtils::dumpGLInfo();

	// Initialization
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	if (debug_m) {
		glDebugMessageCallback(GLUtils::debugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
			GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
	}

}

int SceneRunner::run(std::unique_ptr<Scene> scene)
{
	// Enter the main loop
	mainLoop(window_m, std::move(scene));

	if (debug_m)
		glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
			GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");

	// Close window and terminate GLFW
	glfwTerminate();

	// Exit program
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


void SceneRunner::printHelpInfo(const char* exeFile, std::map<std::string, std::string>& sceneData)
{
	printf("Usage: %s recipe-name\n\n", exeFile);
	printf("Recipe names: \n");
	for (auto it : sceneData) {
		printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
	}
}

void SceneRunner::mainLoop(GLFWwindow* window, std::unique_ptr<Scene> scene)
{

	scene->setDimensions(fbw, fbh);
	scene->initScene();
	scene->resize(fbw, fbh);

	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		GLUtils::checkForOpenGLError(__FILE__, __LINE__);

		scene->update(float(glfwGetTime()));
		scene->render();
		glfwSwapBuffers(window);

		glfwPollEvents();
		int state = glfwGetKey(window, GLFW_KEY_SPACE);
		if (state == GLFW_PRESS)
			scene->animate(!scene->animating());
	}
}
