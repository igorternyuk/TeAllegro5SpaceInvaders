#pragma once

#include <allegro5/allegro_image.h>
#include "utils.hpp"
#include "entity.hpp"

class Bullet : public Entity
{
public:
    enum class Type { Ship, Enemy };
    explicit Bullet(int x, int y, int vel_x, int vel_y, Direction dir,
                    Type type, BitmapManager &bitmapManager);
    int getWidth() const;
    int getHeight() const;
    void draw();
private:
    ALLEGRO_BITMAP *bitmap_;
    int width_, height_;
};

