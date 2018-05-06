#pragma once

#include "utils.hpp"
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"

class Entity
{
public:
    explicit Entity(int x, int y, int vel_x, int vel_y, Direction dir);
    virtual ~Entity();
    int getX() const;
    int getY() const;
    int getVelX() const;
    int getVelY() const;
    bool isActive() const;
    void move(Direction dir);
    void hide();
    bool isHidden();
    void resetPosition();
    void explode();
    void activate();
    virtual void draw() = 0;
private:
    int init_x_, init_y_;
    int x_, y_;
    int velocity_x_, velocity_y_;
    Direction dir_;
    bool isActive_ = true;
    bool isHidden_ = false;
    ALLEGRO_SAMPLE *explosion_;
};
