#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <vector>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define DEBUG_INFO 0

#define BULLET_SPEED        120.0f
#define MAX_SPEED_SPACESHIP 50.0f

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

    // getter functions
    olc::vf2d getPos() const
    {
        return m_pos;
    }
    float getOrientation() const
    {
        return m_angle;
    }
    int getSize() const
    {
        return m_size;
    }

    void init( const olc::vf2d position, const int size, const eObjectType type, const int life = 1 )
    {
        m_vPoints.clear();
        m_pos   = position;
        m_size  = size;
        m_life  = life;
        
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
        if( eObjectType::BULLET == m_type )
        {
            pge.FillCircle( m_pos, 1, m_color );

            return;
        }
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

        // draw acceleration effect
        if( eObjectType::SPACESHIP == m_type && true == m_bAccelerate )
        {
            olc::vf2d dir;
            dir.x = sinf( m_angle );
            dir.y = -cosf( m_angle );
            dir *= -0.5f * m_size;
            pge.FillCircle( m_pos + dir, 2, olc::YELLOW );
        }
#if DEBUG_INFO
        pge.DrawStringDecal( m_pos, "angle: " + std::to_string( m_angle ), olc::WHITE, { 0.5, 0.5 } );
        pge.DrawStringDecal( m_pos + olc::vd2d( 0.0f, 8.0f ), "speed: " + std::to_string( m_velocity.mag() ), olc::WHITE, { 0.5, 0.5 } );
        pge.DrawStringDecal( m_pos + olc::vd2d( 0.0f, 16.0f ), "life: " + std::to_string( m_life ), olc::WHITE, { 0.5, 0.5 } );
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
                m_velocity.x += sinf( m_angle ) * 40.0f * timeElapsed;
                m_velocity.y += -cosf( m_angle ) * 40.0f * timeElapsed;
                                
                m_bAccelerate = true;

                // limit speed
                if( m_velocity.mag() > MAX_SPEED_SPACESHIP )
                {
                    m_velocity *= ( MAX_SPEED_SPACESHIP / m_velocity.mag() );
                    m_bAccelerate = false;
                }
            }

            if( pge.GetKey( olc::Key::UP ).bReleased )
            {
                m_bAccelerate = false;
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

    // used e.g. for destroying bullets (when hitting an asteroid)
    void setPositionToInvalid()
    {
        m_pos.x = -1000;;
    }

    // object is hit, returns true, if life is <= 0 after the hit
    bool hit()
    {
        m_life--;

        return ( m_life <= 0 );
    }
private:
    /* attributes */
    int m_life              = 1;
    float m_angle           = 0.0f;
    float m_angleDelta      = 0.0f;  // for asteroids only, constant rotation speed
    //bool m_bIsPlayer        = true;  // otherwise it is an asteroid
    eObjectType m_type      = eObjectType::ASTEROID;

    olc::vf2d m_pos         = { 100, 100 };

    olc::vf2d m_velocity    = { 0, 0 };

    /* appearance */
    int m_size              = 20; // in pixels (asteroids -> radius)
    olc::Pixel m_color      = olc::GREEN;
    bool m_bAccelerate      = false;    // for drawing effect only

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
        
        createAsteroids( 3, m_player.getPos(), 100, 60, 3, m_vAsteroids );
    }
    void createAsteroids( const int number,
                          const olc::vf2d centerPoint,
                          const float approxDistToCenterPoint,
                          const int size,
                          const int life,
                          std::vector< SpaceObject >& vAsteroids )
    {
        // distance to center point, around which the asteroids will be created
        float distToCP = approxDistToCenterPoint;

        for( int i = 0; i < number; ++i )
        {
            SpaceObject obj;

            // get random angle (around the player) to position the asteroid
            float rndAngle = ( float )rand() / ( float )RAND_MAX * 6.28318f;
            distToCP += ( float )rand() / ( float )RAND_MAX * 10;
            olc::vf2d pos;
            pos.x += sinf( rndAngle ) * distToCP;
            pos.y += -cosf( rndAngle ) * distToCP;

            pos += centerPoint;

            obj.init( pos, size, SpaceObject::eObjectType::ASTEROID, life );

            // random velocity
            float rndDirection = ( float )rand() / ( float )RAND_MAX * 6.28318f;
            olc::vf2d vel;
            vel.x += sinf( rndDirection ) * 25;
            vel.y += -cosf( rndDirection ) * 25;
            obj.setVelocity( vel );

            vAsteroids.push_back( obj );
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
        SpaceObject bullet;

        olc::vf2d vel;
        vel.x = sinf( m_player.getOrientation() );
        vel.y = -cosf( m_player.getOrientation() );

        // place the ball at the front of the spaceship (that's why we shift the position a bit)
        bullet.init( m_player.getPos() + vel * (float)m_player.getSize() * 0.7f, 2, SpaceObject::BULLET );

        vel *= BULLET_SPEED;
        bullet.setVelocity( vel );

        m_vBullets.push_back( bullet );
    }

    void checkForHits()
    {
        // new asteroids after collision are stored here, to not mess up the loop/deleting
        std::vector< SpaceObject > vNewAsteroids;

        for( int i = 0; i < ( int )m_vBullets.size(); ++i )
        {
            auto it = m_vAsteroids.begin();

            while( it != m_vAsteroids.end() )
            {
                // calc distance from bullet to the current asteroid
                const float dist = ( m_vBullets[ i ].getPos() - ( *it ).getPos() ).mag();

                if( dist < ( *it ).getSize() / 2.0f )
                {
                    m_vBullets[ i ].setPositionToInvalid();

                    if( true == ( *it ).hit() )
                    {
                        const int destroyedAsteroidSize = ( *it ).getSize();
                        if( destroyedAsteroidSize > 15 )
                        {
                            const int newSize = destroyedAsteroidSize / 2;
                            int newLife = 2;
                            if( 15 == newSize )
                            {
                                newLife = 1;
                            }
                            createAsteroids( 2, ( *it ).getPos(), (float)newSize / 2.0f, newSize, newLife, vNewAsteroids );
                        }
                        it = m_vAsteroids.erase( it );
                        
                        continue;
                    }
                }

                it++;
            }
        }

        // add new asteroids to main vector
        for( auto a : vNewAsteroids )
        {
            m_vAsteroids.push_back( a );
        }
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
        {
            if( GetKey( olc::Key::SPACE ).bPressed )
            {
                shoot( fElapsedTime );
            }
            for( int i = 0; i < ( int )m_vBullets.size(); ++i )
            {
                m_vBullets[ i ].update( *this, fElapsedTime );
                m_vBullets[ i ].draw( *this );
            }

            //TODO: TEST, remove later!
            if( GetKey( olc::Key::K ).bPressed )
            {
                for( int i = 0; i < ( int )m_vBullets.size(); ++i )
                {
                    m_vBullets[ i ].setPositionToInvalid();
                }
            }

            // Remove bullets that have gone off screen
            if( m_vBullets.size() > 0 )
            {
                auto it = m_vBullets.begin();
                while( it != m_vBullets.end() )
                {
                    if( ( *it ).getPos().x < 1 || ( *it ).getPos().y < 1 || ( *it ).getPos().x >= ScreenWidth() - 1 || ( *it ).getPos().y >= ScreenHeight() - 1 )
                    {
                        it = m_vBullets.erase( it );
                    }
                    else
                    {
                        it++;
                    }
                }
            }

#if DEBUG_INFO
            char text[ 100 ];
            sprintf_s( text, "Number bullets: %d", ( int )m_vBullets.size() );
            DrawStringDecal( { 10.0f, ScreenHeight() - 20.0f }, text, olc::WHITE, { 0.5, 0.5 } );
#endif

            checkForHits();
        }

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