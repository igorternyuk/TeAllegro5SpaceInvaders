#include "shield.hpp"

Shield::Shield(int x, int y, const charMatrix &arrangement, int wallWidth, int wallHeight,
                 BitmapManager &bitmapManager,
                 SampleManager &sampleManager)
{
    for(int i = 0; i < int(arrangement.size()); ++i)
    {
        for(int j = 0; j < int(arrangement[i].size()); ++j)
        {
            WallType currWallType;
            switch(arrangement[i][j])
            {
                case ' ' :
                    continue;
                case 'A' :
                    currWallType = Wall::Type::TOP_LEFT;
                    break;
                case 'D' :
                    currWallType = Wall::Type::LEFT;
                    break;
                case 'C' :
                    currWallType = Wall::Type::TOP_RIGHT;
                    break;
                case 'E' :
                    currWallType = Wall::Type::RIGHT;
                    break;
                case 'B' :
                default :
                    currWallType = Wall::Type::CENTRAL;
                    break;
            }

            auto wall = std::make_unique<Wall>(currWallType, x + j * wallWidth,
                                               y + i * wallHeight,
                                               bitmapManager, sampleManager);
            walls_.push_back(std::move(wall));
        }
    }
}

void Shield::checkCollisions(std::vector<std::unique_ptr<Bullet>> &bullets)
{
    for(auto &bullet : bullets)
    {
        if(!bullet->isActive()) continue;
        for(auto &wall : walls_)
        {
            if(wall->isDestroyed()) continue;
            if(wall->isCollision(bullet))
            {
                wall->damage();
                bullet->explode();
                break;
            }
        }
    }
}

void Shield::draw() const
{
    for(auto &wall : walls_)
        wall->draw();
}
