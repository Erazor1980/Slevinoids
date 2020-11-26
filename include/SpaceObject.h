#pragma once
#include "olcPixelGameEngine.h"

#define DEBUG_INFO 1

#define BULLET_SPEED        120.0f
#define MAX_SPEED_SPACESHIP 50.0f

class SpaceObject
{
public:
    SpaceObject();

    enum eObjectType
    {
        SPACESHIP,
        ASTEROID,
        BULLET
    };

    void init( const olc::vf2d position, const int size, const eObjectType type, const int life = 1 );
    void draw( olc::PixelGameEngine& pge, const bool bDebugInfo = false ) const;   
    void update( const olc::PixelGameEngine& pge, const float timeElapsed );


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

    // setter functions
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
    int m_life = 1;
    float m_angle = 0.0f;
    float m_angleDelta = 0.0f;  // for asteroids only, constant rotation speed
                                //bool m_bIsPlayer        = true;  // otherwise it is an asteroid
    eObjectType m_type = eObjectType::ASTEROID;

    olc::vf2d m_pos = { 100, 100 };

    olc::vf2d m_velocity = { 0, 0 };

    /* appearance */
    int m_size = 20; // in pixels (asteroids -> radius)
    olc::Pixel m_color = olc::GREEN;
    bool m_bAccelerate = false;    // for drawing effect only

    std::vector< olc::vf2d > m_vPoints;
};