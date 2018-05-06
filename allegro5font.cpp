#include "allegro5font.hpp"
#include <memory>

Allegro5Font::Allegro5Font()
{}

bool Allegro5Font::loadFromFile(const std::__cxx11::string &fileName, int size)
{
    mFont.reset(nullptr);
    auto font = al_load_font(fileName.c_str(), size, 0);
    if(!font)
        return false;
    auto ptr = std::make_unique<ALLEGRO_FONT>(font, al_destroy_font);
    mFont.swap(ptr);
    return true;
}

ALLEGRO_FONT *Allegro5Font::get() const
{
    return mFont.get();
}
