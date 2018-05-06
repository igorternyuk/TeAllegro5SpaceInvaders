#pragma once

#include "allegro5/allegro_image.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"

class Bullet;
class Wall
{
public:
    enum class Type { TOP_LEFT, LEFT, TOP_RIGHT, RIGHT, CENTRAL };
    explicit Wall(WallType type, int x, int y, const std::string &pathToBitmap,
                  int width, int height, ALLEGRO_SAMPLE *explosion);
    ~Wall();
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool isCollision(const Bullet *bullet) const;
    void damage();
    bool isDestroyed() const;
    void draw() const;
private:
    const int MAX_HITS_COUNT = 3;
    Type type_;
    int x_, y_;
    ALLEGRO_BITMAP *bitmap_;
    int width_, height_;
    int numHits_;
    ALLEGRO_SAMPLE *explosion_;
    void playExplosionSound();
};
