#pragma once

#include "utils.hpp"

class Hero;
class Bullet;

class AliensWave
{
public:
    explicit AliensWave(int initX, int initY, const charMatrix &arrangement, int vDist, int hDist,
    int velocity_x, int velocity_y, Direction dir, int topLimit, int leftLimit, int bottomLimit, int rightLimit, int strenth,
    std::string pathToBitmap, int a_width, int a_height, int bulletVelocity, std::string pathToBulletBitmap,
    int b_width, int b_height, ALLEGRO_SAMPLE *shot, ALLEGRO_SAMPLE *explosion);
    ~AliensWave();
    void move();
    void shoot(std::vector<Bullet*> &bullets);
    void checkCollisions(const std::vector<Bullet*> &bullets, int &scoreIncrement);
    bool isAlive() const;
    bool isTouchPlanet() const;
    bool isEnoughSpaceForRedUFO() const;
    void draw() const;
private:
    const int MIN_TOP_FOR_ENOUGH_SPACE_FOR_NFO = 157;
    int velocity_x_, velocity_y_;
    Direction dir_;
    int topLimit_, leftLimit_, bottomLimit_, rightLimit_;
    int strenth_;
    int a_width_, a_height_; //alien width and height
    std::vector<Hero*> aliensWave_;
    bool isTouchPlanet_ = false;
};

