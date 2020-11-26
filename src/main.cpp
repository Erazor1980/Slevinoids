#define OLC_PGE_APPLICATION
#include "Slevinorids.h"

int main()
{
    Slevinorids game;
    if( game.Construct( 400, 300, 3, 3 ) )
        game.Start();
    return 0;
}