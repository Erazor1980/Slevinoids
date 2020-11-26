#include "Slevinorids.h"

Slevinorids::Slevinorids()
{
    /* initialize random seed */
    srand( unsigned int( time( NULL ) ) );

    // Name you application
    sAppName = "Slevinorids";
}

bool Slevinorids::OnUserCreate()
{
    // Called once at the start, so create things here
    resetGame();

    return true;
}

bool Slevinorids::OnUserUpdate( float timeElapsed )
{
    if( GetKey( olc::Key::ESCAPE ).bPressed )
    {
        resetGame();
    }
    if( GetKey( olc::Key::I ).bPressed )
    {
        m_bDebugInfo = !m_bDebugInfo;
    }

    if( !m_bGameOver )
    {
        updateGame( timeElapsed );
    }

    composeFrame( m_bDebugInfo );

    return true;
}

void Slevinorids::composeFrame( const bool bDebugInfo )
{
    Clear( olc::BLACK );

    ////// ASTEROIDS //////
    for( int i = 0; i < ( int )m_vAsteroids.size(); ++i )
    {
        m_vAsteroids[ i ].draw( *this, bDebugInfo );


        if( bDebugInfo )
        {
            char text[ 100 ];
            sprintf_s( text, "%d. pos: %f, %f", i, m_vAsteroids[ i ].getPos().x, m_vAsteroids[ i ].getPos().y );
            DrawStringDecal( { 10.0f, i * 10.0f + 5 }, text, olc::WHITE, { 0.5, 0.5 } );
        }
    }

    ////// PLAYER //////
    m_player.draw( *this, bDebugInfo );
    if( bDebugInfo && m_bCollision )
    {
        DrawCircle( m_player.getPos(), m_player.getSize() / 2, olc::RED );
        DrawLine( m_player.getPos(), m_CollisionAsteroidPos, olc::RED );
    }

    ////// BULLETS //////
    for( int i = 0; i < ( int )m_vBullets.size(); ++i )
    {
        m_vBullets[ i ].draw( *this );
    }

    if( bDebugInfo )
    {
        char text[ 100 ];
        sprintf_s( text, "Number bullets: %d", ( int )m_vBullets.size() );
        DrawStringDecal( { 10.0f, ScreenHeight() - 20.0f }, text, olc::WHITE, { 0.5, 0.5 } );
    }

    ////// SCORE //////
    if( !m_bGameOver )
    {
        char text[ 100 ];
        sprintf_s( text, "SCORE: %d", m_score );
        DrawStringDecal( { ScreenWidth() - 90.0f, 10.0f }, text, olc::BLUE, { 0.8f, 0.8f } );
    }

    ///// GAME OVER /////
    if( m_bGameOver )
    {
        gameOverScreen();
    }
}

void Slevinorids::updateGame( const float timeElapsed )
{
    ////// PLAYER //////
    m_player.update( *this, timeElapsed );
    m_bGameOver = checkForCollision();

    ////// ASTEROIDS //////
    for( int i = 0; i < ( int )m_vAsteroids.size(); ++i )
    {
        m_vAsteroids[ i ].update( *this, timeElapsed );
    }

    ////// BULLETS //////
    {
        if( GetKey( olc::Key::SPACE ).bPressed )
        {
            shoot( timeElapsed );
        }
        for( int i = 0; i < ( int )m_vBullets.size(); ++i )
        {
            m_vBullets[ i ].update( *this, timeElapsed );
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

        checkForHits();
    }

    ///// NEXT ROUND /////
    if( m_vAsteroids.empty() )
    {
        // for every new round 1 more asteroid is created
        m_nNewAsteroids++;
        createAsteroids( m_nNewAsteroids, m_player.getPos(), 100, 60, 3, m_vAsteroids );
    }
}

void Slevinorids::resetGame()
{
    m_score = 0;
    m_player.init( { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f }, 20, SpaceObject::eObjectType::SPACESHIP );
    m_player.setVelocity( { 0, 0 } );
    m_timeSinceLastShot = 0.0f;
    m_nNewAsteroids = 3;
    m_bCollision = false;
    m_bGameOver = false;

    m_vAsteroids.clear();
    m_vBullets.clear();

    createAsteroids( 3, m_player.getPos(), 100, 60, m_nNewAsteroids, m_vAsteroids );
}

void Slevinorids::createAsteroids( const int number,
                                   const olc::vf2d centerPoint,
                                   const float approxDistToCenterPoint,
                                   const int size, const int life,
                                   std::vector<SpaceObject>& vAsteroids )
{
    // distance to center point, around which the asteroids will be created
    float distToCP = approxDistToCenterPoint;

    for( int i = 0; i < number; ++i )
    {
        SpaceObject obj;

        // get random angle (around the center point) to position the asteroid
        float rndAngle = ( float )rand() / ( float )RAND_MAX * 6.28318f;
        distToCP += ( float )rand() / ( float )RAND_MAX * 10;
        olc::vf2d pos;
        pos.x += sinf( rndAngle ) * distToCP;
        pos.y += -cosf( rndAngle ) * distToCP;

        pos += centerPoint;

        obj.init( pos, size, SpaceObject::eObjectType::ASTEROID, life );

        // random flying directioon (constant velocity for all asteroids)
        float rndDirection = ( float )rand() / ( float )RAND_MAX * 6.28318f;
        olc::vf2d vel;
        vel.x += sinf( rndDirection ) * 25;
        vel.y += -cosf( rndDirection ) * 25;
        obj.setVelocity( vel );

        vAsteroids.push_back( obj );
    }
}

void Slevinorids::shoot( const float timeElapsed )
{
    SpaceObject bullet;

    olc::vf2d vel;
    vel.x = sinf( m_player.getOrientation() );
    vel.y = -cosf( m_player.getOrientation() );

    // place the ball at the front of the spaceship (that's why we shift the position a bit)
    bullet.init( m_player.getPos() + vel * ( float )m_player.getSize() * 0.7f, 2, SpaceObject::BULLET );

    vel *= BULLET_SPEED;
    bullet.setVelocity( vel );

    m_vBullets.push_back( bullet );
}

void Slevinorids::checkForHits()
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

            // asteroid hit by the bullet
            if( dist < ( *it ).getSize() / 2.0f )
            {
                m_vBullets[ i ].setPositionToInvalid();

                // asteroid destroyed
                if( true == ( *it ).hit() )
                {
                    const int destroyedAsteroidSize = ( *it ).getSize();

                    // create child asteroids
                    if( destroyedAsteroidSize > 15 )
                    {
                        const int newSize = destroyedAsteroidSize / 2;
                        int newLife = 2;
                        if( 15 == newSize )
                        {
                            newLife = 1;
                        }
                        createAsteroids( 2, ( *it ).getPos(), ( float )newSize / 2.0f, newSize, newLife, vNewAsteroids );
                    }

                    // increase points based on the size of destroyed asteroids
                    switch( destroyedAsteroidSize )
                    {
                    case 60:
                        m_score += 5;
                        break;
                    case 30:
                        m_score += 10;
                        break;
                    case 15:
                        m_score += 20;
                        break;
                    default:
                        break;
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

bool Slevinorids::checkForCollision()
{
    // for debug only
    bool bCollision = false;

    // using local variable to update member variable for game over at the end of this function (to not mess up debug info)
    bool bGameOver = false;

    for( const auto& a : m_vAsteroids )
    {
        // distance player to current asteroid
        const float dist = ( a.getPos() - m_player.getPos() ).mag();

        // collision detected
        if( dist < a.getSize() / 2.0f + m_player.getSize() / 3.0f )
        {
            // player dead, game over
            if( m_player.hit() )
            {
                bGameOver = true;
            }
            // for debug only
            bCollision = true;
            m_CollisionAsteroidPos = a.getPos();
        }

    }

    if( true == bCollision )
    {
        m_bCollision = true;
    }
    else
    {
        m_bCollision = false;
    }

    return bGameOver;
}

void Slevinorids::gameOverScreen()
{
    const olc::vf2d pos( 1 / 7.0f * ScreenWidth(), 1 / 3.0f * ScreenHeight() );
    DrawStringDecal( pos, "GAME OVER", olc::MAGENTA, { 4, 4 } );

    char text[ 100 ];
    sprintf_s( text, "FINAL SCORE: %d", m_score );
    DrawStringDecal( pos + olc::vf2d( 70.0f, 50.0f ), text, olc::BLUE, { 1.2f, 1.2f } );
}
