#pragma once

#include "wall.hpp"
#include "bullet.hpp"
#include "utils.hpp"

class Shield
{
public:
    explicit Shield(int x, int y, const charMatrix &arrangement,
                    int wallWidth, int wallHeight,
                     BitmapManager &bitmapManager,
                     SampleManager &sampleManager);
    void checkCollisions(std::vector<std::unique_ptr<Bullet>> &bullets);
    void draw() const;
private :
    std::vector<std::unique_ptr<Wall>> walls_;
};

