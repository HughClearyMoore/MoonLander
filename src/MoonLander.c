#include "MLCore.h"

#define GAME_WIDTH 640
#define GAME_HEIGHT 480


int main(int argc, char** argv)
{
	Game game = GameCreate(GAME_WIDTH, GAME_HEIGHT, "Moon Lander");
	glfwSetWindowUserPointer(game.window, &game);

	GameStart(&game);

	GameDestroy(&game);

	return 0x0;
}