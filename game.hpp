#pragma once

#include "utils.hpp"
#include "allegro5initializer.hpp"
#include <memory>

class Hero;
class Bullet;
class AliensWave;
class Baffles;

class Game
{
public:

    explicit Game();
    ~Game();
    void startPage(bool &startGame);
    void run();
private:
    enum
    {
        WINDOW_WIDTH = 600,
        WINDOW_HEIGHT = 600,
        WINDOW_LEFT = 400,
        WINDOW_TOP = 50,
        INIT_SPACESHIP_X = 300,
        INIT_SPACESHIP_Y = 541,
        SPACESHIP_VELOCITY = 6,
        SPACESHIP_LIVES_NUMBER = 3,
        ALIENS_WAVE_X = 39,
        ALIENS_WAVE_Y = 140,
        ALIENS_HORIZONTAL_OFFSET = 3,
        ALIENS_VERTICAL_OFFSET = 5,
        ALIENS_VELOCITY_X = 8,
        ALIENS_VELOCITY_Y = 16,
        LEFT_LIMIT = 39,
        TOP_LIMIT = 130,
        RIGHT_LIMIT = 556,
        BOTTOM_LIMIT = 545,
        ALIENS_WAVE_STRENTH = 2,
        RED_NFO_VELOCITY = 10,
        FIRST_BAFFLE_LEFT = 108,
        FIRST_BAFFLE_TOP = 466,
        BULLET_VELOCITY = 8
    };


    const std::string WINDOW_TITLE = "SpaceInviders";
    const std::string GAME_PAUSED_TEXT = "GAME PAUSED";
    const std::string LOST_MESSAGE = "YOU LOST";
    const std::string PATH_TO_SETTINGD_FILE = "settings.txt";

    Allegro5Initializer allegro5Initializer_;
    BitmapManager bitmaps_;
    FontManager fonts_;
    SampleManager samples_;

    int redNFO_ticker_ = 0;
    const int MAX_RED_NFO_DELAY = 100;
    int randDelay_ = rand() % MAX_RED_NFO_DELAY;
    const float SPACESHIP_TIMER_TIMEOUT = 0.1f;
    const float SPACESHIP_BULLETS_TIMER_TIMEOUT = 0.05f;
    const float ALIENS_WAVE_TIMER_TIMEOUT = 0.2f;
    const float ALIENS_BULLETS_TIMER_TIMEOUT = 0.05f;
    const float ALIENS_SHOOTING_TIMER_TIMEOUT = 2.0f;

    ALLEGRO_DISPLAY *display_;
    ALLEGRO_BITMAP *background_;
    ALLEGRO_FONT *smallFont_;
    ALLEGRO_FONT *middleFont_;
    ALLEGRO_FONT *largeFont_;
    ALLEGRO_BITMAP *startPage_;
    ALLEGRO_BITMAP *oneLifeBitmap_;
    ALLEGRO_BITMAP *levelFlagBitmap_;
    ALLEGRO_TIMER *spaceshipTimer_;
    ALLEGRO_TIMER *spaceshipBulletsTimer_;
    ALLEGRO_TIMER *aliensWaveTimer_;
    ALLEGRO_TIMER *aliensShootingTimer_;
    ALLEGRO_TIMER *aliensBulletsTimer_;
    ALLEGRO_SAMPLE *shot_;
    ALLEGRO_SAMPLE *explosion_;
    ALLEGRO_SAMPLE *backgroundMusic_;
    ALLEGRO_SAMPLE_INSTANCE *backgroundInstance_;

    Hero *spaceship_;
    Hero *redUFO_;
    AliensWave *aliensWave_;
    Baffles *baffles_;
    std::vector<Bullet*> spaceshipBullets_;
    std::vector<Bullet*> aliensBullets_;
    bool isGameStarted_ = false;
    bool isGameOver_ = false;
    bool isGamePaused_ = false;
    int level_ = 1;
    int score_ = 0;

private:
    void init();
    bool loadSettings(const std::string &pathToSettingsFile, std::string &pathToSprite,
         int &characterWidth, int &characterHeight, charMatrix &aliensArrangement,
         std::string &spaceshipBulletPath, int &spaceShipBulletWidth, int &spaceShipBulletHeight,
         std::string &alienBulletPath, int &alienBulletWidth, int &alienBulletHeight,
         std::string &pathToWallImg, int &wallWidth, int &wallHeight, charMatrix &wallsArrangement);
    void loadBitmaps();
    void loadFonts();
    void loadSamples();
    void createCharacters();
    void moveSpaceshipBullets();
    void moveAliensBullets();
    void prepareNewGame(int level);
    void startAllTimers();
    void stopAllTimers();
    void drawAllBullets() const;
    void drawScore() const;
    void drawSpaceShipLives() const;
    void deleteBullets();
};
