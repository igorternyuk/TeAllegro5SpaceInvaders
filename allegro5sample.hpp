#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "my_untils.hpp"

class Allegro5Sample
{
public:
    explicit Allegro5Sample();
    bool loadFromFile(const std::string &fileName);
    ALLEGRO_SAMPLE* get() const;
private:
    my_unique_ptr<ALLEGRO_SAMPLE> mSample;
};


/*
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <memory>
#include "my_untils.hpp"

class Allegro5Bitmap
{
public:
    explicit Allegro5Bitmap();
    bool loadFromFile(const std::string &fileName);
    ALLEGRO_BITMAP* get() const;
private:
    my_unique_ptr<ALLEGRO_BITMAP> mBitmap;
};

*/
