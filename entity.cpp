#include "entity.hpp"

Entity::Entity(int x, int y, int vel_x, int vel_y, Direction dir) :
        init_x_(x), init_y_(y), x_(x), y_(y), velocity_x_(vel_x),
        velocity_y_(vel_y), dir_(dir)
{}

Entity::~Entity()
{}

int Entity::getX() const
{
    return x_;
}

int Entity::getY() const
{
    return y_;
}

int Entity::getVelX() const
{
    return velocity_x_;
}

int Entity::getVelY() const
{
    return velocity_y_;
}

bool Entity::isActive() const
{
    return isActive_;
}

void Entity::move(Direction dir)
{
    if(isActive())
    {
        dir_ = dir;
        switch(dir_)
        {
            case Direction::LEFT:
                x_ -= velocity_x_;
                break;
            case Direction::RIGHT :
                x_ += velocity_x_;
                break;
            case Direction::UP :
                y_ -= velocity_y_;
                break;
            case Direction::DOWN :
                y_ += velocity_y_;
                break;
        }
    }
}

void Entity::hide()
{
    x_ = -500;
    y_ = -500;
    isHidden_ = true;
}

bool Entity::isHidden()
{
    return isHidden_;
}

void Entity::resetPosition()
{
    x_ = init_x_;
    y_ = init_y_;
    isHidden_ = false;
}

void Entity::explode()
{
    isActive_ = false;
}

void Entity::activate()
{
    isActive_ = true;
}
