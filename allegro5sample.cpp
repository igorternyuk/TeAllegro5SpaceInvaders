#include "allegro5sample.hpp"

Allegro5Sample::Allegro5Sample()
{}

bool Allegro5Sample::loadFromFile(const std::__cxx11::string &fileName)
{
    mSample.reset(nullptr);
    auto sound = al_load_sample(fileName.c_str());
    if(!sound)
        return false;
    auto ptr = std::make_unique<ALLEGRO_SAMPLE>(sound, al_destroy_sample);
    mSample.swap(ptr);
    return true;
}

ALLEGRO_SAMPLE *Allegro5Sample::get() const
{
    return mSample.get();
}
