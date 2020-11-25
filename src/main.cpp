#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class LukisTest : public olc::PixelGameEngine
{
public:
    LukisTest()
    {
        // Name you application
        sAppName = "Luki's Test";
    }

public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override
    {
        Clear( olc::BLACK );

        DrawLine( olc::vi2d( 30, 30 ), olc::vi2d( 280, 80 ), olc::BLUE );
        DrawCircle( olc::vi2d( ScreenWidth() / 2, ScreenHeight() / 2 ), 20, olc::GREEN );
        FillCircle( olc::vi2d( ScreenWidth() / 3, ScreenHeight() / 3 ), 20, olc::RED );
        DrawString( olc::vi2d( 30, 200 ), "Here we go!", olc::DARK_GREEN, 2 );
        DrawStringDecal( olc::vi2d( 30, 250 ), "Here we go!", olc::DARK_GREEN, { 1.2f, 0.5f } );

        const auto mousePos = GetMousePos();

        if( mousePos.x > ScreenWidth() / 2 )
        {
            DrawString( olc::vi2d( ScreenWidth() / 2, 20 ), "RIGHT!", olc::DARK_GREEN, 2 );
        }
        else if( mousePos.x < ScreenWidth() / 2 )
        {
            DrawString( olc::vi2d( 10, 20 ), "LEFT!", olc::DARK_GREEN, 2 );
        }

        //std::string mousePosTxt =;

        return true;
    }
};

int main()
{
    LukisTest demo;
    if( demo.Construct( 400, 300, 2, 2 ) )
        demo.Start();
    return 0;
}