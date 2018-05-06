TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    game.cpp \
    bullet.cpp \
    entity.cpp \
    alienswave.cpp \
    baffles.cpp \
    wall.cpp \
    allegro5font.cpp \
    allegro5bitmap.cpp \
    allegro5sample.cpp \
    allegro5initializer.cpp \
    hero.cpp

LIBS += `pkg-config --libs allegro-5.0 allegro_dialog-5.0 allegro_ttf-5.0 allegro_font-5.0 allegro_primitives-5.0 allegro_image-5.0 allegro_audio-5.0 allegro_acodec-5.0`

HEADERS += \
    alienswave.hpp \
    baffles.hpp \
    bullet.hpp \
    entity.hpp \
    game.hpp \
    utils.hpp \
    wall.hpp \
    resourcemanager.hpp \
    resourceindentifiers.hpp \
    allegro5font.hpp \
    my_untils.hpp \
    allegro5bitmap.hpp \
    allegro5sample.hpp \
    allegro5initializer.hpp \
    hero.hpp

DISTFILES += \
    resourcemanager.impl
