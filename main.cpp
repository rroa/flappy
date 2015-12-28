#include "SDLWrapper.hpp"

#if EMSCRIPTEN
    #include <emscripten.h>
#endif

// Game Window Size
//
const int WIDTH         = 1136;
const int HEIGHT        = 640;

// Game obj
//
Flappy::SDLWrapper* game   = 0;

void step ()
{
    game->OnExecute( );
}

int main(int argc, char *argv[])
{
    game = new Flappy::SDLWrapper( "Flappy Bird Clone", WIDTH, HEIGHT );

    if( !game->OnInit( ))
    {
        std::cout << "Error initializing game\n";
        return 1;
    }

#if EMSCRIPTEN
    emscripten_set_main_loop( step, 0, 0 );
#else
    while( game->IsRunning( ))
    {
        step( );
    }
#endif

    delete game;
    return 0;
}
