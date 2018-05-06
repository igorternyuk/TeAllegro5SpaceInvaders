#include "game.hpp"
#include <stdexcept>
#include <iostream>

int main()
{
    try
    {
        Game game;
        bool mayBeStartGame;
        game.startPage(mayBeStartGame);
        if(mayBeStartGame)
            game.run();
    }
    catch(std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 0;
}
