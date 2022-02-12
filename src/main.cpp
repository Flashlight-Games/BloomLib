#include "dependencies.h"
#include "../Game/engine/engine.h"

#include <chrono> // Time

int main()
{ 
    Game::Engine Engine = Game::Engine(true);

    glfwTerminate();
    return 0;
}