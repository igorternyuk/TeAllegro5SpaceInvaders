#include "allegro5bitmap.hpp"

Allegro5Bitmap::Allegro5Bitmap()
{}

bool Allegro5Bitmap::loadFromFile(const std::__cxx11::string &fileName)
{
    mBitmap.reset(nullptr);
    auto bitmap = al_load_bitmap(fileName.c_str());
    if(!bitmap)
        return false;
    auto ptr = std::make_unique<ALLEGRO_BITMAP>(bitmap, al_destroy_bitmap);
    mBitmap.swap(ptr);
    return true;
}

ALLEGRO_BITMAP *Allegro5Bitmap::get() const
{
    return mBitmap;
}
