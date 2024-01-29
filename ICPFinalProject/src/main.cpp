#include <iostream>

#include "GameApp.h"


int main() {

	GameApp game;
	if (game.run_game() == 0) {
		std::cout << "App is correctly terminated !" << std::endl;
		return 0;
	}
	else {
		std::cout << "There was an error ! Check error output !" << std::endl;
		return -1;
	}

}


