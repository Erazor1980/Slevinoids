#include "SpaceObject.h"

SpaceObject::SpaceObject()
{
}

void SpaceObject::init( const olc::vf2d position, const int size, const eObjectType type, const int life )
{
    m_vPoints.clear();
    m_pos = position;
    m_size = size;
    m_life = life;

    if( eObjectType::SPACESHIP == type )
    {
        /* player as triangle */
        m_vPoints = {
            { 0, -m_size * 2 / 3.0f },              // top
        { -m_size / 4.0f, m_size * 1 / 3.0f },  // bottom left
        { m_size / 4.0f, m_size * 1 / 3.0f },   // bottom right
        };

        m_type = eObjectType::SPACESHIP;
        m_color = olc::WHITE;
    }
    else if( eObjectType::ASTEROID == type )
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

        m_angleDelta = ( float )rand() / ( float )RAND_MAX * 0.00004f + 0.00008f;
        if( rand() % 2 == 1 )
        {
            m_angleDelta *= -1;
        }
        m_type = eObjectType::ASTEROID;
        m_color = olc::DARK_YELLOW;
    }
    else if( eObjectType::BULLET == type )
    {
        m_type = eObjectType::BULLET;
        m_color = olc::DARK_GREY;
    }
}

void SpaceObject::draw( olc::PixelGameEngine& pge, const bool bDebugInfo ) const
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

    if( bDebugInfo )
    {
        pge.DrawStringDecal( m_pos, "angle: " + std::to_string( m_angle ), olc::WHITE, { 0.5, 0.5 } );
        pge.DrawStringDecal( m_pos + olc::vd2d( 0.0f, 8.0f ), "speed: " + std::to_string( m_velocity.mag() ), olc::WHITE, { 0.5, 0.5 } );
        pge.DrawStringDecal( m_pos + olc::vd2d( 0.0f, 16.0f ), "life: " + std::to_string( m_life ), olc::WHITE, { 0.5, 0.5 } );
    }
}

void SpaceObject::update( const olc::PixelGameEngine& pge, const float timeElapsed )
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
    if( m_pos.x >( float )pge.ScreenWidth() + m_size / 2.0f )
    {
        m_pos.x = -m_size / 2.0f;
    }
    if( m_pos.y < -m_size / 2.0f )
    {
        m_pos.y = ( float )pge.ScreenHeight() + m_size / 2.0f;
    }
    if( m_pos.y >( float )pge.ScreenHeight() + m_size / 2.0f )
    {
        m_pos.y = -m_size / 2.0f;
    }
}