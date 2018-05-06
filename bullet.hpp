#pragma once

#include <allegro5/allegro_image.h>
#include "utils.hpp"
#include "entity.hpp"

class Bullet : public Entity
{
public:
    explicit Bullet(int x, int y, int vel_x, int vel_y, Direction dir,
                    ALLEGRO_BITMAP* bitmap, int width, int height);
    int getWidth() const;
    int getHeight() const;
    void draw();
private:
    ALLEGRO_BITMAP *bitmap_;
    int width_, height_;
};

