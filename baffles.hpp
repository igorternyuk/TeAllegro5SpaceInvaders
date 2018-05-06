#pragma once

#include "utils.hpp"

class Wall;
class Bullet;
class Baffles
{
public:
    explicit Baffles(int x, int y, const charMatrix &arrangement, const std::string &pathToWallBitmap,
                     int w_width, int w_height, ALLEGRO_SAMPLE *explosion);
    ~Baffles();
    void checkCollisions(std::vector<Bullet*> bullets);
    void draw() const;
private :
    std::vector<Wall*> walls_;
};

