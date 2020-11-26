#pragma once
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "SpaceObject.h"

// Override base class with your custom functionality
class Slevinorids : public olc::PixelGameEngine
{
public:
    Slevinorids();

    bool OnUserCreate() override;
    bool OnUserUpdate( float timeElapsed ) override;

private:
    struct Explosion
    {
        olc::vf2d m_pos;
        float m_duration = 0.0;
    };
    // draw everything to screen
    void composeFrame( const bool bDebugInfo = false );

    // update all game entities
    void updateGame( const float timeElapsed );

    void resetGame();
    
    void createAsteroids( const int number,
                          const olc::vf2d centerPoint,
                          const float approxDistToCenterPoint,
                          const int size,
                          const int life,
                          std::vector< SpaceObject >& vAsteroids );
    
    // shoot button has been pressed or is held
    void shoot();

    // checks if a bullet hits an asteroid (including erasing and creating of new asteroids)
    void checkForHits();

    // checks if the spaceship collides with an asteroid (true -> game over)
    bool checkForCollision();

    // draws game over 
    void gameOverScreen();

    void updateExplosions( const float timeElapsed );
private:
    SpaceObject m_player;
    std::vector< SpaceObject > m_vAsteroids;
    std::vector< SpaceObject > m_vBullets;
    std::vector< Explosion > m_vExplosions;

    const float m_timeBetweenShots = 0.200f; // in sec
    float m_timeSinceLastShot = 0.0f;

    int m_score = 0;
    int m_level = 1;
    int m_nNewAsteroids = 3;

    bool m_bGameOver = false;

    // for debug info only
    bool m_bDebugInfo = false;
    bool m_bCollision = false;
    olc::vf2d m_CollisionAsteroidPos;
};