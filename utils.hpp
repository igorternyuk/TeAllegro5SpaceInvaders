#pragma once

#include "resourceindentifiers.hpp"
#include "allegro5bitmap.hpp"
#include "allegro5font.hpp"
#include "allegro5sample.hpp"
#include "resourcemanager.hpp"
#include <vector>
#include <string>

using charMatrix = std::vector<std::string>;
using BitmapManager = ResourceManager<Allegro5Bitmap, BitmapID>;
using FontManager = ResourceManager<Allegro5Font, FontID>;
using SampleManager = ResourceManager<Allegro5Sample, SampleID>;
enum class Direction {LEFT, RIGHT, UP, DOWN};


