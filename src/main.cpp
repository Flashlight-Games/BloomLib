#include "../Game/engine/engine.h"

int main()
{ 
    Game::Engine Engine = Game::Engine(true);

    glfwTerminate();
    return 0;
}