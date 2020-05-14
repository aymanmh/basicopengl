#include <iostream>
#include "SceneRunner.h"
#include "BasicScene.h"
#include "SpriteScene.h"
#include "PhongScene.h"

int main()
{
	std::string title = "Test Window";
	SceneRunner myRunner(title,1280,720);
	std::unique_ptr<Scene> scene;

	//scene = std::unique_ptr<Scene>(new BasicScene());
	scene = std::unique_ptr<Scene>(new SpriteScene());
	//scene = std::unique_ptr<Scene>(new PhongScene());
	
	myRunner.setScene(std::move(scene));
	return myRunner.run();
}