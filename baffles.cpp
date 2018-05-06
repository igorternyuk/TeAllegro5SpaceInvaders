#include "baffles.hpp"
#include "wall.hpp"
#include "bullet.hpp"

Baffles::Baffles(int x, int y, const charMatrix &arrangement, const std::string &pathToWallBitmap,
         int w_width, int w_height, ALLEGRO_SAMPLE *explosion)
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
                    currWallType = WallType::TOP_LEFT;
                    break;
                case 'D' :
                    currWallType = WallType::LEFT;
                    break;
                case 'C' :
                    currWallType = WallType::TOP_RIGHT;
                    break;
                case 'E' :
                    currWallType = WallType::RIGHT;
                    break;
                case 'B' :
                default :
                    currWallType = WallType::CENTRAL;
                    break;
            }

            Wall *wall = new Wall(currWallType, x + j * w_width, y + i * w_height,
                                  pathToWallBitmap, w_width, w_height, explosion);
            walls_.push_back(wall);
        }
    }
}

Baffles::~Baffles()
{
    for(auto &wall : walls_)
        if(wall != nullptr)
            delete wall;
}

void Baffles::checkCollisions(std::vector<Bullet*> bullets)
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

void Baffles::draw() const
{
    for(auto &wall : walls_)
        wall->draw();
}
