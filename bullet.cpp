#include "bullet.hpp"

Bullet::Bullet(int x, int y, int vel_x, int vel_y, Direction dir,
               Type type, BitmapManager &bitmapManager) :
    Entity(x, y, vel_x, vel_y, dir)
{
    if(type == Type::Ship){
        bitmap_ = bitmapManager.get(BitmapID::ShipBullet).get();
        width_ = 6;
        height_ = 12;
    } else if(type == Type::Enemy){
        bitmap_ = bitmapManager.get(BitmapID::EnemyBullet).get();
        width_ = 12;
        height_ = 12;
    }
}

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
