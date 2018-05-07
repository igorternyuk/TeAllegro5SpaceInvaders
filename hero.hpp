#pragma once

#include "utils.hpp"
#include "entity.hpp"
#include "bullet.hpp"

#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"

class Hero : public Entity
{
public:
    enum class Type { SPACESHIP, ELEPHANT, ROBOT, ROMBUS, RED_UFO };
    explicit Hero(Type type, int x, int y, int velX,
                  int velY, Direction dir, int width, int height,
                  int numLives, int bulletVelocity,
                  BitmapManager &bitmapManager,
                  SampleManager &sampleManager);
    Type getType() const;
    void shoot(std::vector<std::unique_ptr<Bullet>> &bullets);
    void damage();
    void repair();
    bool isCollision(const Bullet *bullet) const;
    int  getScoreIncrement() const;
    int getWidth() const;
    int getHeight() const;
    int  getNumLives() const;
    void draw();

private:
    enum
    {
        MAX_TICK_ALIVE_COUNT = 30,
        MAX_TICK_EXPLODED_COUNT = 10
    };
    Type type_;
    int width_, height_;
    int numLives_;
    int bulletVelocity_;
    BitmapManager& bitmaps_;
    ALLEGRO_BITMAP *sprite_;
    ALLEGRO_SAMPLE *shot_;
    ALLEGRO_SAMPLE *explosion_;
    int sourceX_, sourceY_;    
    int bulletWidth_, bulletHeight_;
    int tickAlive_ = 0;
    int tickExploded_ = 0;    

private:
    void drawExplosion();
    void playShotSound();
    void playExplosionSound();
};

