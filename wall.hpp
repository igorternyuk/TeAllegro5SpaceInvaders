#pragma once

#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"
#include "utils.hpp"

class Bullet;
class Wall
{
public:
    enum class Type { TOP_LEFT, LEFT, TOP_RIGHT, RIGHT, CENTRAL };
    explicit Wall(Type type, int x, int y, int width, int height,
                  BitmapManager &bitmapManager,
                  SampleManager &sampleManager);
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

    int width_, height_;
    ALLEGRO_BITMAP *bitmap_;
    ALLEGRO_SAMPLE *explosion_;
    int numHits_ = 0;
    void playExplosionSound();
};
