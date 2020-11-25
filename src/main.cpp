#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class SpaceObject
{
public:
    SpaceObject()
    {
        /* player as triangle */
        m_points[ 0 ] = { 0, -m_size * 2 / 3.0f };              // top
        m_points[ 1 ] = { -m_size / 4.0f, m_size * 1 / 3.0f };  // bottom left
        m_points[ 2 ] = { m_size / 4.0f, m_size * 1 / 3.0f };   // bottom right
    }

    void draw( olc::PixelGameEngine& pge )
    {
        // rotate first
        olc::vf2d pointsRotated[ 3 ] = { m_points[ 0 ], m_points[ 1 ], m_points[ 2 ] };
        for( int i = 0; i < 3; ++i )
        {
            const float x = pointsRotated[ i ].x;
            const float y = pointsRotated[ i ].y;

            pointsRotated[ i ].x = x * cosf( m_angle ) - y * sinf( m_angle );
            pointsRotated[ i ].y = y * cosf( m_angle ) + x * sinf( m_angle );
        }

        // translate
        for( int i = 0; i < 3; ++i )
        {
            pointsRotated[ i ] += m_pos;
        }

        pge.DrawTriangle( pointsRotated[ 0 ], pointsRotated[ 1 ], pointsRotated[ 2 ], m_color );

        
    }

    void update( const olc::PixelGameEngine& pge, const float timeElapsed )
    {
        if( pge.GetKey( olc::Key::LEFT ).bHeld )
        {
            m_angle += -timeElapsed * 3;
        }
        if( pge.GetKey( olc::Key::RIGHT ).bHeld )
        {
            m_angle += timeElapsed * 3;
        }


        // Thrust? Apply ACCELERATION
        if( pge.GetKey( olc::Key::UP ).bHeld )
        {
            // ACCELERATION changes VELOCITY (with respect to time)
            m_velocity.x += sin( m_angle ) * 40.0f * timeElapsed;
            m_velocity.y += -cos( m_angle ) * 40.0f * timeElapsed;
        }

        // VELOCITY changes POSITION (with respect to time)
        m_pos += m_velocity * timeElapsed;
        //player.x += player.dx * timeElapsed;
        //player.y += player.dy * timeElapsed;
    }
private:
    /* attributes */
    float m_angle = 0.0f;

    olc::vf2d m_pos = { 100, 100 };

    olc::vf2d m_velocity    = { 0, 0 };

    /* appearance */
    int m_size = 20; // in pixels
    olc::Pixel m_color = olc::GREEN;

    //std::vector< olc::vf2d m_points[ 3 ];
    olc::vf2d m_points[ 3 ];

};

// Override base class with your custom functionality
class LukisTest : public olc::PixelGameEngine
{
public:
    LukisTest()
    {
        // Name you application
        sAppName = "Luki's Test";
    }

    olc::Sprite* mp_img = nullptr;
    olc::Decal* mp_decal = nullptr;

    int m_nLayers;
    SpaceObject m_player;
public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        mp_img = new olc::Sprite( "D:\\Projects\\_images_\\bvb.png" );
        mp_decal = new olc::Decal( mp_img );

        m_nLayers = CreateLayer();



        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override
    {
        Clear( olc::BLACK );

#if 0
        DrawLine( olc::vi2d( 30, 30 ), olc::vi2d( 280, 80 ), olc::BLUE );
        DrawCircle( olc::vi2d( ScreenWidth() / 2, ScreenHeight() / 2 ), 20, olc::GREEN );
        FillCircle( olc::vi2d( ScreenWidth() / 3, ScreenHeight() / 3 ), 20, olc::RED );
        DrawString( olc::vi2d( 30, 200 ), "Here we go!", olc::DARK_GREEN, 2 );
        DrawStringDecal( olc::vi2d( 30, 250 ), "Here we go!", olc::DARK_GREEN, { 1.2f, 0.5f } );

        FillTriangle( { 30, 30 }, { 70, 30 }, { 35, 40 }, olc::DARK_BLUE );

        const auto mousePos = GetMousePos();

        if( mousePos.x > ScreenWidth() / 2 )
        {
            DrawString( olc::vi2d( ScreenWidth() / 2, 20 ), "RIGHT!", olc::DARK_GREEN, 2 );
        }
        else if( mousePos.x < ScreenWidth() / 2 )
        {
            DrawString( olc::vi2d( 10, 20 ), "LEFT!", olc::DARK_GREEN, 2 );
        }

        //DrawSprite( mousePos, mp_img );
        DrawDecal( mousePos, mp_decal );

        auto leftMouseButton = GetMouse( 0 );
        if( leftMouseButton.bHeld )
        {
            DrawCircle( mousePos, 10, olc::GREEN );
        }
        auto rightMouseButton = GetMouse( 1 );
        if( rightMouseButton.bHeld )
        {
            DrawCircle( mousePos, 10, olc::RED );
        }

        //std::string mousePosTxt =;
#endif

        if( GetKey( olc::Key::LEFT ).bHeld )
        {
            //m_player.m_pos.x -= 10 * fElapsedTime;
        }

        m_player.update( *this, fElapsedTime );

        m_player.draw( *this );

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