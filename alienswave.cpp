#include "alienswave.hpp"
#include "hero.hpp"
#include "bullet.hpp"

AliensWave::AliensWave(int initX, int initY, const charMatrix &arrangement, int vDist, int hDist,
    int velocity_x, int velocity_y, Direction dir, int topLimit, int leftLimit, int bottomLimit, int rightLimit, int strenth,
    std::string pathToBitmap, int a_width, int a_height, int bulletVelocity, std::string pathToBulletBitmap,
    int b_width, int b_height, ALLEGRO_SAMPLE *shot, ALLEGRO_SAMPLE *explosion) :
    velocity_x_(velocity_x), velocity_y_(velocity_y), dir_(dir), topLimit_(topLimit),
    leftLimit_(leftLimit), bottomLimit_(bottomLimit), rightLimit_(rightLimit),
    strenth_(strenth), a_width_(a_width), a_height_(a_height)
{
    for(int i = 0; i < int(arrangement.size()); ++i)
    {
        for(int j = 0; j < int(arrangement[i].size()); ++j)
        {
            CharacterType type;
            switch(arrangement[i][j])
            {
                case ' ' :
                    continue;
                case 'D' :
                    type = CharacterType::ROMBUS;
                    break;
                case 'R' :
                    type = CharacterType::ROBOT;
                    break;
                case 'E' :
                    type = CharacterType::ELEPHANT;
                    break;
                default:
                    type = CharacterType::ROBOT;
                    break;
            }
            Hero *alien = new Hero(type, initX + j * (a_width_ + hDist),
            initY + i * (a_height_ + vDist), velocity_x_, velocity_y_, Direction::RIGHT, 1,
            bulletVelocity, pathToBitmap, a_width_, a_height_, pathToBulletBitmap, b_width,
            b_height, shot, explosion);
            aliensWave_.push_back(alien);
        }
    }
}

AliensWave::~AliensWave()
{
    for(auto &alien : aliensWave_)
        delete alien;
}

void AliensWave::move()
{
    for(auto &alien : aliensWave_)
    {
        if(!alien->isActive())
            continue;
        if(alien->getX() <= leftLimit_)
        {
            //std::cout << "Коснулись левой стенки!" << std::endl;
            if(dir_ == Direction::DOWN)
            {
                //std::cout << "Двигались вниз поворачиваем направо" << std::endl;
                dir_ = Direction::RIGHT;
            } else if(dir_ == Direction::LEFT)
            {
               // std::cout << "Двигались влево спускаемся вниз" << std::endl;
                dir_ = Direction::DOWN;
            }
            break;
            //dir_ = (dir_ == Direction::DOWN) ? Direction::RIGHT : Direction::DOWN;
        }
        else if(alien->getX() + a_width_ >= rightLimit_)
        {
            //std::cout << "Коснулись правой стенки!" << std::endl;
            if(dir_ == Direction::DOWN)
            {
                //std::cout << "Двигались вниз поворачиваем налево" << std::endl;
                dir_ = Direction::LEFT;
            } else if(dir_ == Direction::RIGHT)
            {
                //std::cout << "Двигались вправо спускаемся вниз" << std::endl;
                dir_ = Direction::DOWN;
            }
            break;
            //dir_ = (dir_ == Direction::DOWN) ? Direction::LEFT : Direction::DOWN;
        }
        if(alien->getY() + a_height_ >= bottomLimit_)
        {
             isTouchPlanet_ = true;
        }
    }
    if(!isTouchPlanet_)
    {
        for(auto &alien : aliensWave_)
            alien->move(dir_);
    }
}

void AliensWave::shoot(std::vector<Bullet*> &bullets)
{
    if(isAlive())
    {
        for(int i = 0; i < strenth_; ++i)
        {
            int randNum;
            do
            {
                randNum = rand() % aliensWave_.size();
            }while(!aliensWave_[randNum]->isActive());
            aliensWave_[randNum]->shoot(bullets);
        }
    }
}

void AliensWave::checkCollisions(const std::vector<Bullet*> &bullets, int &scoreIncrement)
{
    for(auto &bullet : bullets)
    {
        if(!bullet->isActive()) continue;
        for(auto &alien : aliensWave_)
        {
            if(!alien->isActive()) continue;
            if(alien->isCollision(bullet))
            {
                scoreIncrement += alien->getScoreIncrement();
                alien->damage();
                bullet->explode();
                break;
            }
        }
    }
}

bool AliensWave::isAlive() const
{
    bool result = false;
    for(const auto &alien : aliensWave_)
    {
        if(alien->isActive())
        {
            result = true;
            break;
        }
    }
    return result;
}

bool AliensWave::isTouchPlanet() const
{
    return isTouchPlanet_;
}

bool AliensWave::isEnoughSpaceForRedUFO() const
{
    bool result = true;
    for(auto alien : aliensWave_)
        if(alien->getY() <= MIN_TOP_FOR_ENOUGH_SPACE_FOR_NFO)
        {
            result = false;
            break;
        }
    return result;
}

void AliensWave::draw() const
{
    for(const auto &alien : aliensWave_)
         alien->draw();
}
