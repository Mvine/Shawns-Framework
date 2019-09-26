#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"
#include "Logging.h"


int main() {

	Logger::Init();
	Game* game = new Game();
	game->Run();
	delete game;
	return 0;
	
}
