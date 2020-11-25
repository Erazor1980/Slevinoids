#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define DEBUG_INFO 1

class SpaceObject
{
public:
    SpaceObject()
    {
    }

    enum eObjectType
    {
        SPACESHIP,
        ASTEROID,
        BULLET
    };

    olc::vf2d getPos() const
    {
        return m_pos;
    }

    void init( const olc::vf2d position, const int size, const eObjectType type )
    {
        m_vPoints.clear();
        m_pos   = position;
        m_size  = size;
        
        if( eObjectType::SPACESHIP == type )
        {
            /* player as triangle */
            m_vPoints = {
                { 0, -m_size * 2 / 3.0f },              // top
                { -m_size / 4.0f, m_size * 1 / 3.0f },  // bottom left
                { m_size / 4.0f, m_size * 1 / 3.0f },   // bottom right
            };

            m_type  = eObjectType::SPACESHIP;
            m_color = olc::WHITE;
        }
        else if ( eObjectType::ASTEROID == type )
        {
            // Create a "jagged" circle for the asteroid. It's important it remains
            // mostly circular, as we do a simple collision check against a perfect
            // circle.
            int verts = 20;

            for( int i = 0; i < verts; i++ )
            {
                float noise = ( float )rand() / ( float )RAND_MAX * 0.4f + 0.8f;
                
                olc::vf2d ver;
                ver.x = noise * sinf( ( ( float )i / ( float )verts ) * 6.28318f ) * m_size / 2.0f;
                ver.y = noise * cosf( ( ( float )i / ( float )verts ) * 6.28318f ) * m_size / 2.0f;
                m_vPoints.push_back( ver );
            }

            m_angleDelta    = ( float )rand() / ( float )RAND_MAX * 0.00004f + 0.00008f;
            if( rand() % 2 == 1 )
            {
                m_angleDelta *= -1;
            }
            m_type      = eObjectType::ASTEROID;
            m_color     = olc::DARK_YELLOW;
        }
        else if( eObjectType::BULLET == type )
        {
            m_type  = eObjectType::BULLET;
            m_color = olc::DARK_GREY;
        }
    }

    void draw( olc::PixelGameEngine& pge ) const
    {
        // we do not want to change the points of the model. that's why we create pointsTransformed
        std::vector< olc::vf2d > pointsTransformed;
        const size_t nPoints = m_vPoints.size();

        if( nPoints == 0 )
        {
            return;
        }

        pointsTransformed.resize( nPoints );

        // rotate first
        for( int i = 0; i < nPoints; ++i )
        {
            const float x = m_vPoints[ i ].x;
            const float y = m_vPoints[ i ].y;

            pointsTransformed[ i ].x = x * cosf( m_angle ) - y * sinf( m_angle );
            pointsTransformed[ i ].y = y * cosf( m_angle ) + x * sinf( m_angle );
        }

        // translate
        for( int i = 0; i < nPoints; ++i )
        {
            pointsTransformed[ i ] += m_pos;
        }
        
        // draw closed polygon
        for( int i = 0; i < nPoints + 1; i++ )
        {
            int j = ( i + 1 );

            pge.DrawLine( pointsTransformed[ i % nPoints ], pointsTransformed[ j % nPoints ], m_color );
        }

#if DEBUG_INFO
        pge.DrawStringDecal( m_pos, "angle: " + std::to_string( m_angle ), olc::WHITE, { 0.5, 0.5 } );
#endif
    }

    void update( const olc::PixelGameEngine& pge, const float timeElapsed )
    {
        // orientation
        if( eObjectType::SPACESHIP == m_type )
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
        }
        else
        {
            m_angle += m_angleDelta;
        }

        if( m_angle > 6.28318f )
        {
            m_angle = 0;
        }
        if( m_angle < 0 )
        {
            m_angle = 6.28318f;
        }

        // VELOCITY changes POSITION (with respect to time)
        m_pos += m_velocity * timeElapsed;

        /* bullets will be erased after leaving the game space, no need for further checks here */
        if( eObjectType::BULLET == m_type )
        {
            return;
        }

        // stay within the game space
        if( m_pos.x < -m_size / 2.0f )
        {
            m_pos.x = ( float )pge.ScreenWidth() + m_size / 2.0f;
        }
        if( m_pos.x > ( float )pge.ScreenWidth() + m_size / 2.0f )
        {
            m_pos.x = -m_size / 2.0f;
        }
        if( m_pos.y < -m_size / 2.0f )
        {
            m_pos.y = ( float )pge.ScreenHeight() + m_size / 2.0f;
        }
        if( m_pos.y > ( float )pge.ScreenHeight() + m_size / 2.0f )
        {
            m_pos.y = -m_size / 2.0f;
        }
    }

    void setVelocity( olc::vf2d vel )
    {
        m_velocity = vel;
    }
private:
    /* attributes */
    float m_angle           = 0.0f;
    float m_angleDelta      = 0.0f;  // for asteroids only, constant rotation speed
    //bool m_bIsPlayer        = true;  // otherwise it is an asteroid
    eObjectType m_type      = eObjectType::ASTEROID;

    olc::vf2d m_pos         = { 100, 100 };

    olc::vf2d m_velocity    = { 0, 0 };

    /* appearance */
    int m_size              = 20; // in pixels (asteroids -> radius)
    olc::Pixel m_color      = olc::GREEN;

    std::vector< olc::vf2d > m_vPoints;    
};

// Override base class with your custom functionality
class LukisTest : public olc::PixelGameEngine
{
public:
    LukisTest()
    {
        /* initialize random seed */
        srand( unsigned int( time( NULL ) ) );

        // Name you application
        sAppName = "Luki's Test";
    }

private:
    SpaceObject m_player;
    std::vector< SpaceObject > m_vAsteroids;
    std::vector< SpaceObject > m_vBullets;

    const float m_timeBetweenShots = 200.0f; // in ms
    float m_timeSinceLastShot = 0.0f;

    int m_score = 0;
public:
    void resetGame()
    {
        m_score = 0;
        m_player.init( { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f }, 20, SpaceObject::eObjectType::SPACESHIP );
        m_player.setVelocity( { 0, 0 } );
        m_timeSinceLastShot = 0.0f;

        m_vAsteroids.clear();
        m_vBullets.clear();
        
        createAsteroids( 3, m_player.getPos() );
    }
    void createAsteroids( const int number, olc::vf2d playerPos )
    {
        float distanceToShip = 100;

        for( int i = 0; i < number; ++i )
        {
            SpaceObject obj;

            // get random angle (around the player) to position the asteroid
            float rndAngle = ( float )rand() / ( float )RAND_MAX * 6.28318f;
            distanceToShip += ( float )rand() / ( float )RAND_MAX * 10;
            olc::vf2d pos;
            pos.x += sin( rndAngle ) * distanceToShip;
            pos.y += -cos( rndAngle ) * distanceToShip;

            pos += playerPos;

            obj.init( pos, 60, SpaceObject::eObjectType::ASTEROID );

            // random velocity
            float rndDirection = ( float )rand() / ( float )RAND_MAX * 6.28318f;
            olc::vf2d vel;
            vel.x += sin( rndDirection ) * 25;
            vel.y += -cos( rndDirection ) * 25;
            obj.setVelocity( vel );

            m_vAsteroids.push_back( obj );
        }
    }

    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        resetGame();

        return true;
    }

    void shoot( const float timeElapsed )
    {
        
    }

    bool OnUserUpdate( float fElapsedTime ) override
    {
        if( GetKey( olc::Key::ESCAPE ).bPressed )
        {
            resetGame();
        }

        Clear( olc::BLACK );
        
        ////// PLAYER //////
        m_player.update( *this, fElapsedTime );
        m_player.draw( *this );

        ////// ASTEROIDS //////
        for( int i = 0; i < (int)m_vAsteroids.size(); ++i )
        {
            m_vAsteroids[ i ].update( *this, fElapsedTime );
            m_vAsteroids[ i ].draw( *this );

#if DEBUG_INFO
            char text[ 100 ];
            sprintf_s( text, "%d. pos: %f, %f", i, m_vAsteroids[ i ].getPos().x, m_vAsteroids[ i ].getPos().y );
            DrawStringDecal( { 10.0f, i * 10.0f + 5 }, text, olc::WHITE, { 0.5, 0.5 } );
#endif
        }

        ////// BULLETS //////
        if( GetKey( olc::Key::SPACE ).bHeld )
        {

        }
#if DEBUG_INFO
        char text[ 100 ];
        sprintf_s( text, "Number bullets: %d", (int)m_vBullets.size() );
        DrawStringDecal( { 10.0f, ScreenHeight() - 20.0f }, text, olc::WHITE, { 0.5, 0.5 } );
#endif

        ////// SCORE //////
        {
            char text[ 100 ];
            sprintf_s( text, "SCORE: %d", m_score );
            DrawStringDecal( { ScreenWidth() - 90.0f, 10.0f }, text, olc::BLUE, { 0.8f, 0.8f } );
        }

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