#include "wall.hpp"
#include "bullet.hpp"

Wall::Wall(Type type, int x, int y, int width, int height,
           BitmapManager &bitmapManager,
           SampleManager &sampleManager) :
      type_(type), x_(x), y_(y),
      width_(width), height_(height),
      bitmap_(bitmapManager.get(BitmapID::Walls)),
      explosion_(sampleManager.get(SampleID::Explosion))
{}

int Wall::getX() const
{
    return x_;
}

int Wall::getY() const
{
    return y_;
}

int Wall::getWidth() const
{
    return width_;
}

int Wall::getHeight() const
{
    return height_;
}

bool Wall::isCollision(const Bullet *bullet) const
{
    bool result;
    if(!isDestroyed())
    {
        int b_x = bullet->getX(), b_y = bullet->getY();
        int b_w = bullet->getWidth(), b_h = bullet->getHeight();
        result = b_x >= x_ - b_w && b_x <= x_ + width_ &&
                 b_y >= y_ - b_h && b_y <= y_ + height_;
    }
    else
        result = false;
    return result;
}

void Wall::damage()
{
    ++numHits_;
    if(numHits_ <= MAX_HITS_COUNT)
        playExplosionSound();
}

bool Wall::isDestroyed() const
{
    return numHits_ >= 3;
}

void Wall::draw() const
{
    if(numHits_ < MAX_HITS_COUNT)
        al_draw_bitmap_region(bitmap_, numHits_ * width_,
                              static_cast<int>(type_) * height_,
                              width_, height_, x_, y_, 0);
}

void Wall::playExplosionSound()
{
    //if(explosion_ != nullptr)
        al_play_sample(explosion_, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}
