#include "alienswave.hpp"

AliensWave::AliensWave(int initX, int initY, const charMatrix &arrangement,
                       int vStep, int hStep, int velocity_x, int velocity_y,
                       Direction dir, int topLimit, int leftLimit,
                       int bottomLimit, int rightLimit, int strenth,
                       int a_width, int a_height, int bulletVelocity,
                       BitmapManager &bitmapManager,
                       SampleManager &sampleManager) :
    velocity_x_(velocity_x), velocity_y_(velocity_y),
    a_width_(a_width), a_height_(a_height), dir_(dir),
    topLimit_(topLimit), leftLimit_(leftLimit),
    bottomLimit_(bottomLimit),
    rightLimit_(rightLimit), strenth_(strenth)
{
    for(int i = 0; i < int(arrangement.size()); ++i)
    {
        for(int j = 0; j < int(arrangement[i].size()); ++j)
        {
            Hero::Type type;
            switch(arrangement[i][j])
            {
                case ' ' :
                    continue;
                case 'D' :
                    type = Hero::Type::ROMBUS;
                    break;
                case 'R' :
                    type = Hero::Type::ROBOT;
                    break;
                case 'E' :
                    type = Hero::Type::ELEPHANT;
                    break;
                default:
                    type = Hero::Type::ROBOT;
                    break;
            }
            //TODO smart pointer
            auto ax = initX + j * (a_width_ + hStep);
            auto ay = initY + i * (a_height_ + vStep);
            auto alien = std::make_unique<Hero>(
                type, ax, ay, velocity_x_, velocity_y_,
                Direction::RIGHT, a_width_, a_height_,
                1, bulletVelocity, bitmapManager,
                sampleManager);
            aliensWave_.push_back(std::move(alien));
        }
    }
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

void AliensWave::shoot(std::vector<std::unique_ptr<Bullet> > &bullets)
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

void AliensWave::checkCollisions(const std::vector<std::unique_ptr<Bullet>> &bullets,
                                 int &scoreIncrement)
{
    for(auto &bullet : bullets)
    {
        if(!bullet->isActive()) continue;
        for(auto &alien : aliensWave_)
        {
            if(!alien->isActive()) continue;
            if(alien->isCollision(bullet.get()))
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
    for(auto &alien : aliensWave_)
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
