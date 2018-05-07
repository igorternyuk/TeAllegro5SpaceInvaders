#pragma once

#include "utils.hpp"
#include <memory>
#include "hero.hpp"
#include "bullet.hpp"

class AliensWave
{
public:
    explicit AliensWave(int initX, int initY, const charMatrix &arrangement,
                        int vStep, int hStep, int velocity_x, int velocity_y,
                        Direction dir, int topLimit, int leftLimit,
                        int bottomLimit, int rightLimit, int strenth,
                        int a_width, int a_height,
                        int bulletVelocity, BitmapManager &bitmapManager,
                        SampleManager &sampleManager);
    void move();
    void shoot(std::vector<std::unique_ptr<Bullet>> &bullets);
    void checkCollisions(const std::vector<std::unique_ptr<Bullet>> &bullets,
                         int &scoreIncrement);
    bool isAlive() const;
    bool isTouchPlanet() const;
    bool isEnoughSpaceForRedUFO() const;
    void draw() const;
private:
    enum { MIN_TOP_FOR_ENOUGH_SPACE_FOR_NFO = 157 };
    int velocity_x_, velocity_y_;
    int a_width_, a_height_; //alien width and height
    Direction dir_;
    int topLimit_, leftLimit_, bottomLimit_, rightLimit_;
    int strenth_;    
    std::vector<std::unique_ptr<Hero>> aliensWave_;
    bool isTouchPlanet_ = false;
};

