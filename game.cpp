#include "game.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Game::Game():
    upDisplay_{ al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT),
                al_destroy_display },
    eventQueue_ { al_create_event_queue(), al_destroy_event_queue }
{
    display_ = upDisplay_.get();
    if(!display_)
    {
         throw std::runtime_error("Could not create Allegro Window");
    }    
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_window_position(display_, WINDOW_LEFT, WINDOW_TOP);
    al_set_window_title(display_, WINDOW_TITLE);

    //Load resources
    loadBitmaps();
    loadFonts();
    loadSamples();

    //Load background music
    my_unique_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance
    {
        al_create_sample_instance(samples_.get(SampleID::Bgm).get()),
        al_destroy_sample_instance
    };

    upBackgroundInstance_.swap(bgmInstance);
    backgroundInstance_ = upBackgroundInstance_.get();
    al_set_sample_instance_playmode(backgroundInstance_ , ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(backgroundInstance_ , al_get_default_mixer());

    //Create timers and game objects
    createTimers();
    createGameObjects();

    //Register event sources
    for(auto &pair: timers_){
        al_register_event_source(eventQueue_.get(), pair.second->getEventSource());
    }
    al_register_event_source(eventQueue_.get(),
                             al_get_display_event_source(display_));
    al_register_event_source(eventQueue_.get(),
                             al_get_keyboard_event_source());
}

void Game::run()
{
    startAllTimers();

    //Main loop of the game

    al_play_sample_instance(backgroundInstance_);

    //bool isRunning = true;
    while(isRunning)
    {
        updatePhase();
        renderPhase();
    }
    stopAllTimers();
}


void Game::updatePhase()
{
    ALLEGRO_EVENT events;
    al_wait_for_event(eventQueue_.get(), &events);
    al_get_keyboard_state(&keyState_);
    draw = false;
    if(events.type == ALLEGRO_EVENT_KEY_UP)
    {
        if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            isRunning = false;
        }
        else if(events.keyboard.keycode == ALLEGRO_KEY_SPACE)
        {
            togglePause();
        }
        else if(events.keyboard.keycode == ALLEGRO_KEY_N)
        {
            startNewGame(1);
            startAllTimers();
            al_play_sample_instance(backgroundInstance_);
        }
        else if(events.keyboard.keycode == ALLEGRO_KEY_F)
        {
            if(gameState_ == GameState::PLAY)
            {
                spaceship_->shoot(spaceshipBullets_);
                draw = true;
            }
        }
        draw = true;
    }
    else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        isRunning = false;
        stopAllTimers();
    }
    else if(events.type == ALLEGRO_EVENT_TIMER)
    {
        if(events.timer.source ==
                timers_.at(TimerID::spaceShip).get()->get())
        {
            if(al_key_down(&keyState_, ALLEGRO_KEY_LEFT))
            {
                if(spaceship_->getX() >= LEFT_LIMIT)
                    spaceship_->move(Direction::LEFT);
            }
            else if(al_key_down(&keyState_, ALLEGRO_KEY_RIGHT))
            {
                if(spaceship_->getX() <= RIGHT_LIMIT - spaceship_->getWidth())
                    spaceship_->move(Direction::RIGHT);
            }
        }
        else if(events.timer.source ==
                timers_.at(TimerID::spaceShipBullets).get()->get())
        {
            moveSpaceshipBullets();
            shield_->checkCollisions(spaceshipBullets_);
            aliensWave_->checkCollisions(spaceshipBullets_, score_);
        }
        else if(events.timer.source ==
                timers_.at(TimerID::alienWave).get()->get())
        {
            aliensWave_->move();
            ++redNFO_ticker_;
            if(aliensWave_->isEnoughSpaceForRedUFO()
               && redUFO_->isHidden()
               && redNFO_ticker_ > randDelay_)
                redUFO_->resetPosition();
            if(redUFO_->isActive() && !redUFO_->isHidden())
                redUFO_->move(Direction::RIGHT);
            if(redUFO_->getX() + redUFO_->getWidth() >= RIGHT_LIMIT - 10)
            {
                redUFO_->hide();
                redNFO_ticker_ = 0;
                randDelay_ = rand() % MAX_RED_NFO_DELAY;
            }
            if(aliensWave_->isTouchPlanet())
                gameState_ = GameState::GAME_OVER;
        }
        else if(events.timer.source ==
                timers_.at(TimerID::alienBullets).get()->get())
        {
            moveAliensBullets();
            shield_->checkCollisions(aliensBullets_);
        }
        else if(events.timer.source ==
                timers_.at(TimerID::alienWaveShooting).get()->get())
        {
            aliensWave_->shoot(aliensBullets_);
        }

        clearInactiveObjects();

        if(gameState_ == GameState::GAME_OVER)
        {
            stopAllTimers();
            al_stop_sample_instance(backgroundInstance_);
        }
        else if(!aliensWave_->isAlive())
                startNewGame(++level_);
        draw = true;
    }
}

void Game::clearInactiveObjects()
{
    auto spaceship_bullets_iterator =
            std::remove_if(spaceshipBullets_.begin(),
                           spaceshipBullets_.end(),
                           [](auto &bullet)
                           {
                                return !bullet->isActive();
                           }
    );
    spaceshipBullets_.erase(spaceship_bullets_iterator, spaceshipBullets_.end());

    //std::cout << "spaceshipBullets_.size() = " << spaceshipBullets_.size() << std::endl;
    auto aliens_bullets_iterator
            = std::remove_if(aliensBullets_.begin(),
                             aliensBullets_.end(),
                             [](auto &bullet)
                              {
                                  return !bullet->isActive();
                              });
    aliensBullets_.erase(aliens_bullets_iterator, aliensBullets_.end());

    //std::cout << "aliensBullets_.size() = " << aliensBullets_.size() << std::endl;
}

void Game::renderPhase()
{
    if(draw)
    {
        al_draw_bitmap(bitmaps_.get(BitmapID::Background).get(),
                       0, 0, 0);
        drawScore();
        drawSpaceShipLives();
        shield_->draw();
        spaceship_->draw();
        aliensWave_->draw();
        redUFO_->draw();
        drawAllBullets();
        if(gameState_ == GameState::PAUSE)
            al_draw_text(fonts_.get(FontID::Large).get(),
                         al_map_rgb(255, 255, 0),
                         100, 270, 0, GAME_PAUSED_TEXT.c_str());
        else if(gameState_ == GameState::GAME_OVER)
                al_draw_text(fonts_.get(FontID::Large).get(),
                             al_map_rgb(255, 0, 0), 150, 270,
                             0, LOST_MESSAGE.c_str());
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }
}

bool Game::loadSettings()
{
    static const std::string pathToSettingsFile { "settings.txt" };
    bool result = false;
    enum loadState
    {
        SPRITE_PATH,
        CHARACTER_WIDTH,
        CHARACTER_HEIGHT,
        ALIENS_ARRANGEMENT,
        SPACESHIP_BULLET_PATH,
        SPACESHIP_BULLET_WIDTH,
        SPACESHIP_BULLET_HEIGHT,
        ALIEN_BULLET_PATH,
        ALIEN_BULLET_WIDTH,
        ALIEN_BULLET_HEIGHT,
        WALL_IMG_PATH,
        WALL_WIDTH,
        WALL_HEIGHT,
        WALLS_ARRANGEMENT
    };

    loadState state = SPRITE_PATH;
    std::ifstream fi(pathToSettingsFile);
    if(!aliensArrangement.empty())
        aliensArrangement.clear();
    if(!wallsArrangement.empty())
        wallsArrangement.clear();
    if(fi.is_open())
    {
        while(!fi.eof())
        {
            std::string line;
            std::getline(fi, line);
            if(line.length() == 0) continue;
            if(line == "[charactersSprite]")
            {
                state = SPRITE_PATH;
                continue;
            }
            else if(line == "[charaterWidth]")
            {
                state = CHARACTER_WIDTH;
                continue;
            }
            else if(line == "[characterHeight]")
            {
                state = CHARACTER_HEIGHT;
                continue;
            }
            else if(line == "[aliensArrangement]")
            {
                state = ALIENS_ARRANGEMENT;
                continue;
            }
            else if(line == "[spaceshipBulletPath]")
            {
                state = SPACESHIP_BULLET_PATH;
                continue;
            }
            else if(line == "[spaceShipBulletWidth]")
            {
                state = SPACESHIP_BULLET_WIDTH;
                continue;
            }
            else if(line == "[spaceShipBulletHeight]")
            {
                state = SPACESHIP_BULLET_HEIGHT;
                continue;
            }
            else if(line == "[alienBulletPath]")
            {
                state = ALIEN_BULLET_PATH;
                continue;
            }
            else if(line == "[alienBulletWidth]")
            {
                state = ALIEN_BULLET_WIDTH;
                continue;
            }
            else if(line == "[alienBulletHeight]")
            {
                state = ALIEN_BULLET_HEIGHT;
                continue;
            }
            else if(line == "[wallImagePath]")
            {
                state = WALL_IMG_PATH;
                continue;
            }
            else if(line == "[wallWidth]")
            {
                state = WALL_WIDTH;
                continue;
            }
            else if(line == "[wallHeight]")
            {
                state = WALL_HEIGHT;
                continue;
            }
            else if(line == "[wallsArrangement]")
            {
                state = WALLS_ARRANGEMENT;
                continue;
            }
            std::stringstream ss;
            switch(state)
            {
                case SPRITE_PATH :
                    pathToSprite = line;
                    break;
                case CHARACTER_WIDTH :
                    ss << line;
                    ss >> heroWidth;
                    break;
                case CHARACTER_HEIGHT :
                    ss << line;
                    ss >> heroHeight;
                    break;
                case ALIENS_ARRANGEMENT :
                    aliensArrangement.push_back(line);
                    break;
                case SPACESHIP_BULLET_PATH :
                    spaceshipBulletPath = line;
                    break;
                case SPACESHIP_BULLET_WIDTH :
                    ss << line;
                    ss >> spaceShipBulletWidth;
                    break;
                case SPACESHIP_BULLET_HEIGHT :
                    ss << line;
                    ss >> spaceShipBulletHeight;
                    break;
                case ALIEN_BULLET_PATH :
                    alienBulletPath = line;
                    break;
                case ALIEN_BULLET_WIDTH :
                    ss << line;
                    ss >> alienBulletWidth;
                    break;
                case ALIEN_BULLET_HEIGHT :
                    ss << line;
                    ss >> alienBulletHeight;
                    break;
                case WALL_IMG_PATH :
                    pathToWallImg = line;
                    break;
                case WALL_WIDTH :
                    ss << line;
                    ss >> wallWidth;
                    break;
                case WALL_HEIGHT :
                    ss << line;
                    ss >> wallHeight;
                    break;
                case WALLS_ARRANGEMENT :
                    wallsArrangement.push_back(line);
                    break;
            }
        }
        fi.close();
        result = !aliensArrangement.empty() && !wallsArrangement.empty();
    }
    else
    {
        al_show_native_message_box(nullptr, nullptr, nullptr,
                                   "Could not open the settings file", nullptr, 0);
        result = false;
    }
    return result;
}

void Game::loadBitmaps()
{
    bitmaps_.load(BitmapID::StartPage, "Resources/Images/start_page.png");
    bitmaps_.load(BitmapID::Background, "Resources/Images/background.png");
    bitmaps_.load(BitmapID::Heroes, "Resources/Images/heroes.png");
    bitmaps_.load(BitmapID::ShipBullet, "Resources/Images/ship_bullet.png");
    bitmaps_.load(BitmapID::ShipLife, "Resources/Images/one_life.png");
    bitmaps_.load(BitmapID::EnemyBullet, "Resources/Images/enemy_bullet.png");
    bitmaps_.load(BitmapID::Walls, "Resources/Images/walls.png");
    bitmaps_.load(BitmapID::LevelFlag, "Resources/Images/level_flag.png");
}

void Game::loadFonts()
{
    static const std::string PATH_TO_FONT = "Resources/Fonts/DroidSansMono.ttf";
    fonts_.load(FontID::Small, PATH_TO_FONT, 18);
    fonts_.load(FontID::Middle, PATH_TO_FONT, 30);
    fonts_.load(FontID::Large, PATH_TO_FONT, 60);
}

void Game::loadSamples()
{
    static const std::string PATH_TO_BACKGROUND_MUSIC = "Resources/Sounds/dart.wav";
    static const std::string PATH_TO_SHOT_SOUND = "Resources/Sounds/shot.wav";
    static const std::string PATH_TO_EXPLOSION = "Resources/Sounds/explosion.wav";
    samples_.load(SampleID::Bgm, PATH_TO_BACKGROUND_MUSIC);
    samples_.load(SampleID::Shot, PATH_TO_SHOT_SOUND);
    samples_.load(SampleID::Explosion, PATH_TO_EXPLOSION);
    al_reserve_samples(4);
}

void Game::createTimers()
{
    auto spaceshipTimer =
            std::make_unique<Allegro5Timer>(SPACESHIP_TIMER_TIMEOUT);
    timers_.insert(std::make_pair(TimerID::spaceShip,
                                  std::move(spaceshipTimer)));

    auto spaceshipBulletsTimer =
            std::make_unique<Allegro5Timer>(SPACESHIP_BULLETS_TIMER_TIMEOUT);
    timers_.insert(std::make_pair(TimerID::spaceShipBullets,
                                  std::move(spaceshipBulletsTimer)));

    auto aliensWaveTimer =
            std::make_unique<Allegro5Timer>(ALIENS_WAVE_TIMER_TIMEOUT);
    timers_.insert(std::make_pair(TimerID::alienWave,
                                  std::move(aliensWaveTimer)));

    auto aliensBulletsTimer =
            std::make_unique<Allegro5Timer>(ALIENS_BULLETS_TIMER_TIMEOUT);
    timers_.insert(std::make_pair(TimerID::alienBullets,
                                  std::move(aliensBulletsTimer)));

    auto aliensShootingTimer =
            std::make_unique<Allegro5Timer>(ALIENS_SHOOTING_TIMER_TIMEOUT);
    timers_.insert(std::make_pair(TimerID::alienWaveShooting,
                                  std::move(aliensShootingTimer)));
}

void Game::createGameObjects()
{
    //Загружаем настройки
    loadSettings();

    //Create spaceship
    spaceship_ = std::make_unique<Hero>(Hero::Type::SPACESHIP,
                          INIT_SPACESHIP_X, INIT_SPACESHIP_Y,
                          SPACESHIP_VELOCITY, 0, Direction::RIGHT,
                          heroWidth, heroHeight, SPACESHIP_LIVES_NUMBER,
                          BULLET_VELOCITY, bitmaps_, samples_);

    //Create alien wave
    aliensWave_ = std::make_unique<AliensWave>(ALIENS_WAVE_X, ALIENS_WAVE_Y,
                           aliensArrangement, ALIEN_WAVE_VERTICAL_STEP,
                           ALIEN_WAVE_HORIZONTAL_STEP, ALIENS_VELOCITY_X,
                           ALIENS_VELOCITY_Y, Direction::RIGHT, TOP_LIMIT,
                           LEFT_LIMIT, BOTTOM_LIMIT, RIGHT_LIMIT, level_ + 1,
                           heroWidth , heroHeight,
                           BULLET_VELOCITY, bitmaps_, samples_);

    //Create the red NFO
    redUFO_ = std::make_unique<Hero>(Hero::Type::RED_UFO, ALIENS_WAVE_X,
                             ALIENS_WAVE_Y, RED_NFO_VELOCITY, 0, Direction::RIGHT,
                             heroWidth + 9, heroHeight, 1, BULLET_VELOCITY,
                             bitmaps_, samples_);
    redUFO_->hide();
    shield_ = std::make_unique<Shield>(FIRST_BAFFLE_LEFT, FIRST_BAFFLE_TOP,
                                       wallsArrangement, wallWidth, wallHeight,
                                       bitmaps_, samples_);
}

void Game::moveSpaceshipBullets()
{
    for(auto &b : spaceshipBullets_)
    {
        if(!b->isActive()) continue;
        b->move(Direction::UP);
        if(redUFO_->isActive() && redUFO_->isCollision(b.get()))
        {
            score_ += redUFO_->getScoreIncrement();
            b->explode();
            redUFO_->explode();
            continue;
        }
        if(b->getY() <= TOP_LIMIT)
            b->explode();
    }
}

void Game::moveAliensBullets()
{
    for(auto &b : aliensBullets_)
    {
        if(!b->isActive()) continue;
        b->move(Direction::DOWN);
        if(spaceship_->isCollision(b.get()))
        {
            b->explode();
            spaceship_->damage();
            if(!spaceship_->isActive())
            {
                gameState_ = GameState::GAME_OVER;
                break;
            }
        }
        else if(b->getY() >= BOTTOM_LIMIT)
            b->explode();
    }
}

void Game::startNewGame(int level)
{

    level_ = level;
    if(level_ == 1)
        score_ = 0;
    createGameObjects();
    redNFO_ticker_ = 0;
    randDelay_ = rand() % MAX_RED_NFO_DELAY;
    isRunning = true;
    gameState_ = GameState::PLAY;
}

void Game::togglePause()
{
    if(gameState_ != GameState::GAME_OVER)
    {
        if(gameState_ == GameState::PLAY)
        {
            gameState_ = GameState::PAUSE;
            stopAllTimers();
            al_stop_sample_instance(backgroundInstance_);
        }
        else if(gameState_ == GameState::PAUSE)
        {
            gameState_ = GameState::PLAY;
            startAllTimers();
            al_play_sample_instance(backgroundInstance_);
        }
    }
}

void Game::startAllTimers()
{
    for(auto &pair: timers_)
        pair.second->start();
}

void Game::stopAllTimers()
{
    for(auto &pair: timers_)
        pair.second->stop();
}

void Game::drawAllBullets() const
{
    for(auto &b : spaceshipBullets_)
        b->draw();
    for(auto &b : aliensBullets_)
        b->draw();
}

void Game::drawScore() const
{
    al_draw_bitmap(bitmaps_.get(BitmapID::LevelFlag).get(), 6, 15, 0);
    al_draw_text(fonts_.get(FontID::Small).get(),
                 al_map_rgb(255, 0, 0), 39, 15, 0,
                 std::to_string(level_).c_str());
    std::stringstream ss;
    ss << "Score " << score_;
    al_draw_text(fonts_.get(FontID::Small).get(), al_map_rgb(255, 0, 0),
                 6, 39, 0, ss.str().c_str());
}

void Game::drawSpaceShipLives() const
{
    al_draw_text(fonts_.get(FontID::Small).get(), al_map_rgb(0,255,0),
                 470, 20, 0, "Ship lives:");
    for(int i = 0; i < spaceship_->getNumLives(); ++i)
    {
        al_draw_bitmap(bitmaps_.get(BitmapID::ShipLife).get(),
                       470 + i * (27 + 3), 51, 0);
    }
}

void Game::startPage(bool &startGame)
{
    Allegro5Timer blinkTimer(0.1f);
    ALLEGRO_KEYBOARD_STATE keyState;
    my_unique_ptr<ALLEGRO_EVENT_QUEUE> event_queue{ al_create_event_queue(),
                                                     al_destroy_event_queue };

    al_register_event_source(event_queue.get(),
                             al_get_display_event_source(display_));
    al_register_event_source(event_queue.get(),
                             al_get_timer_event_source(blinkTimer.get()));
    al_register_event_source(event_queue.get(),
                             al_get_keyboard_event_source());

    bool showStartPage {true};
    int sourceX {1}, sourceY {0};
    int pageRegionWidth = 600, pageRegionHeight = 400;
    bool showInstructions {false};
    const int delay {30};
    int counter {0};
    al_start_timer(blinkTimer.get());

    while(showStartPage)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap_region(bitmaps_.get(BitmapID::StartPage).get(),
                              sourceX * pageRegionWidth,
                              sourceY * pageRegionHeight,
                              pageRegionWidth, pageRegionHeight, 0, 0, 0);
        al_flip_display();

        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue.get(), &events);
        al_get_keyboard_state(&keyState);

        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            if(events.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                startGame = true;
                showInstructions = true;
                sourceY = 1;
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                showStartPage = false;
                startGame = false;
            }
        }
        else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            showStartPage = false;
            startGame = false;
        }
        else if(events.type == ALLEGRO_EVENT_TIMER)
        {
            if(events.timer.source == blinkTimer.get())
                sourceX = (sourceX == 0) ? 1 : 0;
            if(showInstructions)
            {
                if(++counter >= delay)
                {
                    showStartPage = false;
                }
            }
        }
    }
}
