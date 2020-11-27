#define OLC_PGE_APPLICATION
#include "Slevinoids.h"

int main()
{
    Slevinoids game;
    if( game.Construct( 400, 300, 3, 3 ) )
        game.Start();
    return 0;
}