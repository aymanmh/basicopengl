// basicopengl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SceneRunner.h"
#include "BasicScene.h"

int main()
{
	std::string title = "Test Window";
	SceneRunner myRunner(title,1280,720);
	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new SceneBasic());

	return myRunner.run(std::move(scene));
}