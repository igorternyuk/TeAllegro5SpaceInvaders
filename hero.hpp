#pragma once

#include "utils.hpp"
#include "entity.hpp"

#include "allegro5/allegro_image.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"

class Bullet;

class Hero : public Entity
{
public:
    enum class Type { SPACESHIP, ELEPHANT, ROBOT, ROMBUS, RED_UFO };
    explicit Hero(CharacterType type, int x, int y, int velX,
                       int velY, Direction dir, int numLives,
                       int bulletVelocity, std::string pathToBitmap,
                       int bitmapWidth, int bitmapHeight,
                       std::string pathToBulletBitmap,
                       int bulletBitmapWidth, int bulletBitmapHeight,
                       ALLEGRO_SAMPLE *shot, ALLEGRO_SAMPLE *explosion);
    CharacterType getType() const;
    void shoot(std::vector<Bullet*> &bullets);
    void damage();
    void repair();
    bool isCollision(const Bullet *bullet) const;
    int  getScoreIncrement() const;
    int getWidth() const;
    int getHeight() const;
    int  getNumLives() const;
    void draw();
private:
    Type type_;
    int numLives_;
    int bulletVelocity_;
    int sourceX_, sourceY_;
    int bitmapWidth_, bitmapHeight_;
    std::string pathToBulletBitmap_;
    int bulletBitmapWidth_, bulletBitmapHeight_;
    ALLEGRO_BITMAP *sprite_;
    ALLEGRO_SAMPLE *shot_;
    ALLEGRO_SAMPLE *explosion_;
    int tickAlive_ = 0;
    int tickExploded_ = 0;
    const int MAX_TICK_ALIVE_COUNT = 30;
    const int MAX_TICK_EXPLODED_COUNT = 10;
    void drawExplosion();
    void playShotSound();
    void playExplosionSound();
};

