#include <QApplication>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "app_window.h"
#include "game.h"
#include "game_window.h"
#include "geometry.h"
#include "entity.h"
#include "file_utils.h"
#include "sprite_set_builder.h"
#include "sprite_set_builder_impl.h"

const double GAME_FPS = 30.0;
const double ANIMATIONS_FPS = 10.0;

void setupGame(Game * game, SpriteSetBuilder& builder) {
    auto assetPaths = listFiles("/Users/curzel/dev/bit-therapy/PetsAssets", ".png");
    auto spriteSets = builder.spriteSets(assetPaths);
    auto apeSprites = spriteSets["ape"];

    auto frame = Rect(0.0, 0.0, 50.0, 50.0);

    Species apeSpecies("ape", 1.0, 1.0);
    Entity ape(ANIMATIONS_FPS, 50.0, 1.0, apeSpecies, apeSprites, frame);

    game->add(ape);
}

std::thread startGameLoop(Game * game) {
    return std::thread([game]() {
        bool gameIsRunning = true;
        auto frameDuration = std::chrono::milliseconds(uint32_t(1000 / GAME_FPS));
        
        while (gameIsRunning) { 
            auto frameStart = std::chrono::steady_clock::now();
            game->update(frameDuration);
            auto frameEnd = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
            
            auto sleepDuration = frameDuration - elapsedTime;
            if (sleepDuration > std::chrono::milliseconds(0)) {
                std::this_thread::sleep_for(sleepDuration);
            }
        }
    });
}

int main(int argc, char *argv[]) {
    std::cout << "Starting..." << std::endl;

    SpriteSetBuilderImpl builderImpl = SpriteSetBuilderImpl({});    

    Game game(GAME_FPS);
    setupGame(&game, builderImpl);    
    auto gameLoop = startGameLoop(&game);
    // gameLoop.join();

    QApplication app(argc, argv);    

    Rect frame(0, 0, 1920, 1080);

    GameWindow gameWindow;
    gameWindow.setup(&game, frame);
    gameWindow.show();

    return app.exec();
}