#include "hero.hpp"

Hero::Hero(Type type, int x, int y, int velX,
           int velY, Direction dir, int width, int height, int numLives,
           int bulletVelocity, BitmapManager &bitmapManager,
           SampleManager &sampleManager) :
    Entity(x, y, velX, velY, dir), type_(type),
    width_(width), height_(height), numLives_(numLives),
    bulletVelocity_(bulletVelocity), bitmaps_(bitmapManager),
    sprite_(bitmapManager.get(BitmapID::Heroes).get()),
    shot_(sampleManager.get(SampleID::Shot).get()),
    explosion_(sampleManager.get(SampleID::Explosion).get()),
    sourceX_(0), sourceY_(static_cast<int>(type_))
{}

Hero::Type Hero::getType() const
{
    return type_;
}

void Hero::shoot(std::vector<std::unique_ptr<Bullet> > &bullets)
{
    Direction shotDir;
    Allegro5Bitmap bulletBitmap;
    int bulletWidth, bulletHeight;

    if(type_ == Type::SPACESHIP)
    {
        shotDir = Direction::UP;
        bulletWidth = bitmaps_.get(BitmapID::ShipBullet).width();
        bulletHeight = bitmaps_.get(BitmapID::ShipBullet).height();
    }
    else
    {
        shotDir = Direction::DOWN;
        bulletWidth = bitmaps_.get(BitmapID::EnemyBullet).width();
        bulletHeight = bitmaps_.get(BitmapID::EnemyBullet).height();
    }

    int bulletX = getX() + (width_ - bulletWidth) / 2;
    int bulletY = (type_ == Type::SPACESHIP)
            ? getY() - height_ - bulletHeight
            : getY() + height_;
    Bullet::Type bType = type_ == Type::SPACESHIP
            ? Bullet::Type::Ship
            : Bullet::Type::Enemy;
    auto bullet = std::make_unique<Bullet>(bulletX, bulletY, 0,
                                           bulletVelocity_, shotDir,
                                           bType, bitmaps_);
    bullets.push_back(std::move(bullet));
    playShotSound();
}

void Hero::damage()
{
    --numLives_;
    playExplosionSound();
    if(numLives_ <= 0)
        explode();
}

void Hero::repair()
{
    numLives_ = 3;
    activate();
}

bool Hero::isCollision(const Bullet *bullet) const
{
    int b_x = bullet->getX(), b_y = bullet->getY();
    int b_w = bullet->getWidth(), b_h = bullet->getHeight();
    return b_x >= this->getX() - b_w && b_x <= this->getX() + width_ &&
           b_y >= this->getY() - b_h && b_y <= this->getY() + height_;
}

int Hero::getScoreIncrement() const
{
    int result;
    if(isActive())
    {
        switch(type_)
        {
            case Type::SPACESHIP :
                 result = 0;
                 break;
            case Type::RED_UFO :
                 result = 150;
                 break;
            case Type::ELEPHANT :
                 result = 70;
                 break;
            case Type::ROBOT :
                 result = 50;
                 break;
            case Type::ROMBUS :
                 result = 30;
                 break;
        }
    }
    else result = 0;
    return result;
}

int Hero::getWidth() const
{
    return width_;
}

int Hero::getHeight() const
{
    return height_;
}

int Hero::getNumLives() const
{
    return numLives_;
}

void Hero::draw()
{
    if(isActive())
    {
        al_draw_bitmap_region(sprite_, sourceX_ * width_,
                              sourceY_ * height_,
                              width_, height_,
                              getX(), getY(), 0);
        switch(type_)
        {
            case Type::SPACESHIP :
            case Type::RED_UFO :
                break;
            case Type::ELEPHANT :
            case Type::ROBOT :
            case Type::ROMBUS :
                ++tickAlive_;
                if(tickAlive_ % 10 == 0)
                {
                    ++sourceX_;
                    if(sourceX_ > 2)
                        sourceX_ = 0;
                }
                if(tickAlive_ >= MAX_TICK_ALIVE_COUNT)
                    tickAlive_ = 0;
                break;
        }
    }
    else
        drawExplosion();
}

void Hero::drawExplosion()
{
    switch(type_)
    {
        case Type::SPACESHIP :
        case Type::RED_UFO :
             sourceX_ = 1;
             break;
        case Type::ELEPHANT :
        case Type::ROBOT :
        case Type::ROMBUS :
             sourceX_ = 3;
             break;
    }
    if(tickExploded_ < MAX_TICK_EXPLODED_COUNT)
    {
        al_draw_bitmap_region(sprite_, sourceX_ * width_,
                              sourceY_ * height_,
                              width_, height_,
                              getX(), getY(), 0);
        ++tickExploded_;
    }
}

void Hero::playShotSound()
{

    if(shot_ != nullptr)
        al_play_sample(shot_, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}

void Hero::playExplosionSound()
{
    if(explosion_ != nullptr)
        al_play_sample(explosion_, 1.0f, 0.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}
