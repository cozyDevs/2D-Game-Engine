#include <iostream>
#include "Game.h"



int main(int argc, char* argv[]) {
    //TODO: Game Loop
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

   
    return 0;
}




