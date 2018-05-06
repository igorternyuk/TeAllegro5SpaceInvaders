#include "game.hpp"
#include "hero.hpp"
#include "alienswave.hpp"
#include "bullet.hpp"
#include "shield.hpp"
#include <stdexcept>

Game::Game()
{
    loadBitmaps();
    loadFonts();
    loadSamples();

    backgroundMusic_ = al_load_sample(PATH_TO_BACKGROUND_MUSIC.c_str());
    backgroundInstance_ = al_create_sample_instance(backgroundMusic_);
    al_set_sample_instance_playmode(backgroundInstance_ , ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(backgroundInstance_ , al_get_default_mixer());
    //Создаем таймеры
    spaceshipTimer_ = al_create_timer(SPACESHIP_TIMER_TIMEOUT);
    spaceshipBulletsTimer_ = al_create_timer(SPACESHIP_BULLETS_TIMER_TIMEOUT);
    aliensWaveTimer_ = al_create_timer(ALIENS_WAVE_TIMER_TIMEOUT);
    aliensBulletsTimer_ = al_create_timer(ALIENS_BULLETS_TIMER_TIMEOUT);
    aliensShootingTimer_ = al_create_timer(ALIENS_SHOOTING_TIMER_TIMEOUT);
    createCharacters();
}

Game::~Game()
{
    al_destroy_timer(spaceshipTimer_);
    al_destroy_timer(aliensWaveTimer_);
    al_destroy_timer(spaceshipBulletsTimer_);
    al_destroy_timer(aliensShootingTimer_);
    al_destroy_timer(aliensBulletsTimer_);
    al_destroy_sample(shot_);
    al_destroy_sample(explosion_);
    al_destroy_sample(backgroundMusic_);
    al_destroy_sample_instance(backgroundInstance_);
    //al_destroy_timer(redUFOTimer_);
    deleteBullets();
    delete spaceship_;
    delete aliensWave_;
    delete redUFO_;
    delete baffles_;
}

void Game::run()
{
    ALLEGRO_KEYBOARD_STATE keyState;
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_timer_event_source(spaceshipTimer_));
    al_register_event_source(event_queue, al_get_timer_event_source(spaceshipBulletsTimer_));
    al_register_event_source(event_queue, al_get_timer_event_source(aliensWaveTimer_));
    al_register_event_source(event_queue, al_get_timer_event_source(aliensShootingTimer_));
    al_register_event_source(event_queue, al_get_timer_event_source(aliensBulletsTimer_));
    al_register_event_source(event_queue, al_get_display_event_source(display_));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    startAllTimers();

    //Main loop of the game

    bool done = false;
    al_play_sample_instance(backgroundInstance_);
    while(!done)
    {
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);
        al_get_keyboard_state(&keyState);
        bool draw = false;
        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                done = true;
                break;
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                if(!isGameOver_)
                {
                    isGamePaused_ = !isGamePaused_;
                    if(isGamePaused_)
                    {
                        stopAllTimers();
                        al_stop_sample_instance(backgroundInstance_);
                    }
                    else
                    {
                        startAllTimers();
                        al_play_sample_instance(backgroundInstance_);
                    }
                }
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_N)
            {
                prepareNewGame(1);
                startAllTimers();
                al_play_sample_instance(backgroundInstance_);
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_F)
            {
                if(!isGamePaused_ && !isGameOver_)
                {
                    spaceship_->shoot(spaceshipBullets_);
                    draw = true;
                }
            }
            draw = true;
        }
        else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
            stopAllTimers();
        }
        else if(events.type == ALLEGRO_EVENT_TIMER)
        {
            if(events.timer.source == spaceshipTimer_)
            {
                if(al_key_down(&keyState, ALLEGRO_KEY_LEFT))
                {
                    if(spaceship_->getX() >= LEFT_LIMIT)
                        spaceship_->move(Direction::LEFT);
                }
                else if(al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
                {
                    if(spaceship_->getX() <= RIGHT_LIMIT - spaceship_->getWidth())
                        spaceship_->move(Direction::RIGHT);
                }
            }
            else if(events.timer.source == spaceshipBulletsTimer_)
            {
                moveSpaceshipBullets();
                baffles_->checkCollisions(spaceshipBullets_);
                aliensWave_->checkCollisions(spaceshipBullets_, score_);
            }
            else if(events.timer.source == aliensWaveTimer_)
            {
                aliensWave_->move();
                ++redNFO_ticker_;
                if(aliensWave_->isEnoughSpaceForRedUFO() && redUFO_->isHidden() && redNFO_ticker_ > randDelay_)
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
                    isGameOver_ = true;
            }
            else if(events.timer.source == aliensShootingTimer_)
            {
                aliensWave_->shoot(aliensBullets_);
            }
            else if(events.timer.source == aliensBulletsTimer_)
            {
                moveAliensBullets();
                baffles_->checkCollisions(aliensBullets_);
            }
            //Удаляем взорвавшиеся снаряды чтобы слишком не накапливались в векторах
            auto spaceship_bullets_iterator = std::remove_if(spaceshipBullets_.begin(), spaceshipBullets_.end(),
                                   [](Bullet *bullet)
                                    {
                                        bool isExploded = !bullet->isActive();
                                        if(isExploded) delete bullet;
                                        return isExploded;
                                    });
            spaceshipBullets_.erase(spaceship_bullets_iterator, spaceshipBullets_.end());

            //std::cout << "spaceshipBullets_.size() = " << spaceshipBullets_.size() << std::endl;
            auto aliens_bullets_iterator = std::remove_if(aliensBullets_.begin(), aliensBullets_.end(),
                                   [](Bullet *bullet)
                                    {
                                        bool isExploded = !bullet->isActive();
                                        if(isExploded) delete bullet;
                                        return isExploded;
                                    });
            aliensBullets_.erase(aliens_bullets_iterator, aliensBullets_.end());

            //std::cout << "aliensBullets_.size() = " << aliensBullets_.size() << std::endl;
            if(isGameOver_)
            {
                stopAllTimers();
                al_stop_sample_instance(backgroundInstance_);
            }
            else if(!aliensWave_->isAlive())
                    prepareNewGame(++level_);
            draw = true;
        }
        if(draw)
        {
            al_draw_bitmap(background_, 0, 0, 0);
            drawScore();
            drawSpaceShipLives();
            baffles_->draw();
            spaceship_->draw();
            aliensWave_->draw();
            redUFO_->draw();
            drawAllBullets();
            if(isGamePaused_ && !isGameOver_)
                al_draw_text(largeFont_, al_map_rgb(255, 255, 0), 100, 270, 0, GAME_PAUSED_TEXT.c_str());
            else if(isGameOver_)
                    al_draw_text(largeFont_, al_map_rgb(255, 0, 0), 150, 270, 0, LOST_MESSAGE.c_str());
            al_flip_display();
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
    }
    al_destroy_event_queue(event_queue);
    stopAllTimers();
}

bool Game::loadSettings(const std::string &pathToSettingsFile, std::string &pathToSprite,
     int &characterWidth, int &characterHeight, charMatrix &aliensArrangement,
     std::string &spaceshipBulletPath, int &spaceShipBulletWidth, int &spaceShipBulletHeight,
     std::string &alienBulletPath, int &alienBulletWidth, int &alienBulletHeight,
     std::string &pathToWallImg, int &wallWidth, int &wallHeight, charMatrix &wallsArrangement)
{
    bool result = false;
    enum loadState {SPRITE_PATH, CHARACTER_WIDTH, CHARACTER_HEIGHT, ALIENS_ARRANGEMENT,
    SPACESHIP_BULLET_PATH, SPACESHIP_BULLET_WIDTH, SPACESHIP_BULLET_HEIGHT, ALIEN_BULLET_PATH,
    ALIEN_BULLET_WIDTH, ALIEN_BULLET_HEIGHT, WALL_IMG_PATH, WALL_WIDTH, WALL_HEIGHT, WALLS_ARRANGEMENT};
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
                    ss >> characterWidth;
                    break;
                case CHARACTER_HEIGHT :
                    ss << line;
                    ss >> characterHeight;
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
        al_show_native_message_box(nullptr, nullptr, nullptr, "Could not open the settings file", nullptr, 0);
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
    al_reserve_samples(50);
}

void Game::createCharacters()
{
    std::string pathToSprite;
    int characterWidth, characterHeight;
    charMatrix aliensArrangement;
    std::string spaceshipBulletPath;
    int spaceShipBulletWidth, spaceShipBulletHeight;
    std::string alienBulletPath;
    int alienBulletWidth, alienBulletHeight;
    std::string pathToWallImg;
    int wallWidth, wallHeight;
    charMatrix wallsArrangement;
    //Загружаем настройки
    loadSettings(PATH_TO_SETTINGD_FILE, pathToSprite, characterWidth, characterHeight, aliensArrangement,
    spaceshipBulletPath, spaceShipBulletWidth, spaceShipBulletHeight, alienBulletPath,
    alienBulletWidth, alienBulletHeight, pathToWallImg, wallWidth, wallHeight, wallsArrangement);
    //Создаем корабль
    spaceship_ = new Hero(CharacterType::SPACESHIP, INIT_SPACESHIP_X, INIT_SPACESHIP_Y,
    SPACESHIP_VELOCITY, 0, Direction::RIGHT, SPACESHIP_LIVES_NUMBER, BULLET_VELOCITY, pathToSprite,
    characterWidth, characterHeight, spaceshipBulletPath, spaceShipBulletWidth, spaceShipBulletHeight,
    shot_, explosion_);
    //Создаем волну инопланетян
    aliensWave_ = new AliensWave(ALIENS_WAVE_X, ALIENS_WAVE_Y, aliensArrangement,
    ALIENS_VERTICAL_OFFSET, ALIENS_HORIZONTAL_OFFSET, ALIENS_VELOCITY_X, ALIENS_VELOCITY_Y,
    Direction::RIGHT, TOP_LIMIT, LEFT_LIMIT, BOTTOM_LIMIT, RIGHT_LIMIT, level_ + 1,
    pathToSprite, characterWidth , characterHeight, BULLET_VELOCITY, alienBulletPath,
    alienBulletWidth, alienBulletHeight, shot_, explosion_);
    //Создаем красный кораблик
    redUFO_ = new Hero(CharacterType::RED_UFO, ALIENS_WAVE_X, ALIENS_WAVE_Y,
    RED_NFO_VELOCITY, 0, Direction::RIGHT, 1, BULLET_VELOCITY, pathToSprite, characterWidth + 9,
    characterHeight, alienBulletPath, alienBulletWidth, alienBulletHeight, shot_, explosion_);
    redUFO_->hide();
    //Создаем щиты
    baffles_ = new Shield(FIRST_BAFFLE_LEFT, FIRST_BAFFLE_TOP, wallsArrangement, pathToWallImg, wallWidth, wallHeight, explosion_);
}

void Game::moveSpaceshipBullets()
{
    for(auto &b : spaceshipBullets_)
    {
        if(!b->isActive()) continue;
        b->move(Direction::UP);
        if(redUFO_->isActive() && redUFO_->isCollision(b))
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
        if(spaceship_->isCollision(b))
        {
            b->explode();
            spaceship_->damage();
            if(!spaceship_->isActive())
            {
                isGameOver_ = true;
                break;
            }
        }
        else if(b->getY() >= BOTTOM_LIMIT)
            b->explode();
    }
}

void Game::prepareNewGame(int level)
{
    isGameOver_ = false;
    isGamePaused_ = false;
    level_ = level;
    if(level_ == 1)
        score_ = 0;
    delete spaceship_;
    delete aliensWave_;
    delete baffles_;
    delete redUFO_;
    deleteBullets();
    createCharacters();
    redNFO_ticker_ = 0;
    randDelay_ = rand() % MAX_RED_NFO_DELAY;
}

void Game::startAllTimers()
{
    al_start_timer(spaceshipTimer_);
    al_start_timer(spaceshipBulletsTimer_);
    al_start_timer(aliensWaveTimer_);
    al_start_timer(aliensShootingTimer_);
    al_start_timer(aliensBulletsTimer_);
    //al_start_timer(redUFOTimer_);
}

void Game::stopAllTimers()
{
    al_stop_timer(spaceshipTimer_);
    al_stop_timer(spaceshipBulletsTimer_);
    al_stop_timer(aliensWaveTimer_);
    al_stop_timer(aliensShootingTimer_);
    al_stop_timer(aliensBulletsTimer_);
    //al_stop_timer(redUFOTimer_);
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
    al_draw_bitmap(levelFlagBitmap_, 6, 15, 0);
    std::stringstream ss1;
    ss1 << level_;
    al_draw_text(smallFont_, al_map_rgb(255, 0, 0), 39, 15, 0, ss1.str().c_str());
    std::stringstream ss2;
    ss2 << "Score " << score_;
    al_draw_text(smallFont_, al_map_rgb(255, 0, 0), 6, 39, 0, ss2.str().c_str());
}

void Game::drawSpaceShipLives() const
{
    for(int i = 0; i < spaceship_->getNumLives(); ++i)
    {
        al_draw_bitmap(oneLifeBitmap_, 470 + i * (27 + 3), 51, 0);
    }
}

void Game::startPage(bool &startGame)
{
    ALLEGRO_TIMER *blinkTimer = al_create_timer(0.1f);
    ALLEGRO_KEYBOARD_STATE keyState;
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display_));
    al_register_event_source(event_queue, al_get_timer_event_source(blinkTimer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    bool flag = false;
    int sourceX = 1, sourceY = 0;
    int pageRegionWidth = 600, pageRegionHeight = 400;
    bool showInstructions = false;
    const int delay = 30;
    int counter = 0;
    al_start_timer(blinkTimer);
    while(!flag)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap_region(startPage_, sourceX * pageRegionWidth,
        sourceY * pageRegionHeight, pageRegionWidth, pageRegionHeight, 0, 0, 0);
        al_flip_display();
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);
        al_get_keyboard_state(&keyState);
        if(events.type == ALLEGRO_EVENT_KEY_UP)
        {
            if(events.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                startGame = true;
                sourceY = 1;
                showInstructions = true;
            }
            else if(events.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                flag = true;
                startGame = false;
            }
        }
        else if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            flag = true;
            startGame = false;
        }
        else if(events.type == ALLEGRO_EVENT_TIMER)
        {
            if(events.timer.source == blinkTimer)
                sourceX = (sourceX == 0) ? 1 : 0;
            if(showInstructions)
            {
                if(++counter >= delay) flag = true;
            }
        }
    }
    al_destroy_timer(blinkTimer);
    al_destroy_event_queue(event_queue);
}

void Game::deleteBullets()
{
    if(!spaceshipBullets_.empty())
    {
        for(auto & b : spaceshipBullets_)
            delete b;
        spaceshipBullets_.clear();
    }
    if(!aliensBullets_.empty())
    {
        for(auto & b : aliensBullets_)
            delete b;
        aliensBullets_.clear();
    }
}
