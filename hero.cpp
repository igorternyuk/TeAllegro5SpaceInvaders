#include "hero.hpp"
#include "bullet.hpp"

Hero::Hero(CharacterType type, int x, int y, int velX, int velY,
           Direction dir, int numLives, int bulletVelocity,
           std::string pathToBitmap, int bitmapWidth, int bitmapHeight,
           std::string pathToBulletBitmap, int bulletBitmapWidth,
           int bulletBitmapHeight, ALLEGRO_SAMPLE *shot,
           ALLEGRO_SAMPLE *explosion) :
    Entity(x, y, velX, velY, dir), type_(type),
    numLives_(numLives), bulletVelocity_(bulletVelocity),
    sourceX_(0), sourceY_(static_cast<int>(type_)), bitmapWidth_(bitmapWidth), bitmapHeight_(bitmapHeight),
    pathToBulletBitmap_(pathToBulletBitmap), bulletBitmapWidth_(bulletBitmapWidth),
    bulletBitmapHeight_(bulletBitmapHeight), sprite_(al_load_bitmap(pathToBitmap.c_str())),
    shot_(shot), explosion_(explosion)
{}

Hero::Type Hero::getType() const
{
    return type_;
}

void Hero::shoot(std::vector<Bullet*> &bullets)
{
    Direction shotDir = (type_ == CharacterType::SPACESHIP) ? Direction::UP : Direction::DOWN;
    int bulletX = getX() + (bitmapWidth_ - bulletBitmapWidth_) / 2;
    int bulletY = (type_ == CharacterType::SPACESHIP) ?
    getY() - bitmapHeight_ - bulletBitmapHeight_ : getY() + bitmapHeight_;
    //TODO change to unique_ptr
    Bullet *bullet = new Bullet(bulletX, bulletY, 0, bulletVelocity_, shotDir, pathToBulletBitmap_,
    bulletBitmapWidth_, bulletBitmapHeight_);
    bullets.push_back(bullet);
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
    return b_x >= this->getX() - b_w && b_x <= this->getX() + bitmapWidth_ &&
           b_y >= this->getY() - b_h && b_y <= this->getY() + bitmapHeight_;
}

int Hero::getScoreIncrement() const
{
    int result;
    if(isActive())
    {
        switch(type_)
        {
            case CharacterType::SPACESHIP :
                 result = 0;
                 break;
            case CharacterType::RED_UFO :
                 result = 150;
                 break;
            case CharacterType::ELEPHANT :
                 result = 70;
                 break;
            case CharacterType::ROBOT :
                 result = 50;
                 break;
            case CharacterType::ROMBUS :
                 result = 30;
                 break;
        }
    }
    else result = 0;
    return result;
}

int Hero::getWidth() const
{
    return bitmapWidth_;
}

int Hero::getHeight() const
{
    return bitmapHeight_;
}

int Hero::getNumLives() const
{
    return numLives_;
}

void Hero::draw()
{
    if(isActive())
    {
        al_draw_bitmap_region(sprite_, sourceX_ * bitmapWidth_, sourceY_ * bitmapHeight_,
        bitmapWidth_, bitmapHeight_, getX(), getY(), 0);
        switch(type_)
        {
            case CharacterType::SPACESHIP :
            case CharacterType::RED_UFO :
                break;
            case CharacterType::ELEPHANT :
            case CharacterType::ROBOT :
            case CharacterType::ROMBUS :
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
        case CharacterType::SPACESHIP :
        case CharacterType::RED_UFO :
             sourceX_ = 1;
             break;
        case CharacterType::ELEPHANT :
        case CharacterType::ROBOT :
        case CharacterType::ROMBUS :
             sourceX_ = 3;
             break;
    }
    if(tickExploded_ < MAX_TICK_EXPLODED_COUNT)
    {
        al_draw_bitmap_region(sprite_, sourceX_ * bitmapWidth_, sourceY_ * bitmapHeight_,
        bitmapWidth_, bitmapHeight_, getX(), getY(), 0);
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
