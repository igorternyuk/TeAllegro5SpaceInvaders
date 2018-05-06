#include "bullet.hpp"

Bullet::Bullet(int x, int y, int vel_x, int vel_y, Direction dir,
               ALLEGRO_BITMAP *bitmap, int width, int height) :
    Entity(x, y, vel_x, vel_y, dir), bitmap_(bitmap),
          width_(width), height_(height)
{}

int Bullet::getWidth() const
{
    return width_;
}

int Bullet::getHeight() const
{
    return height_;
}

void Bullet::draw()
{
    if(isActive())
        al_draw_bitmap_region(bitmap_, 0, 0, width_, height_, getX(), getY(), 0);
}
